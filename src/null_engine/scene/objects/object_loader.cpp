#include "object_loader.hpp"

#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/format.h>

#include <cstddef>
#include <iostream>
#include <null_engine/drawable_objects/common/vertex.hpp>
#include <null_engine/drawable_objects/material/texture.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <null_engine/util/geometry/matrix.hpp>
#include <optional>
#include <string>

namespace null_engine {

namespace {

Vec3 VectorFromAssimp(const aiVector3D& vector) {
    return Vec3(vector.x, vector.y, vector.z);
}

Transform TransformFromAssimp(const aiMatrix4x4& matrix) {
    return Transform(Mat4({
        {matrix.a1, matrix.a2, matrix.a3, matrix.a4},
        {matrix.b1, matrix.b2, matrix.b3, matrix.b4},
        {matrix.c1, matrix.c2, matrix.c3, matrix.c4},
        {matrix.d1, matrix.d2, matrix.d3, matrix.d4},
    }));
}

}  // anonymous namespace

ObjectLoader::ObjectLoader(const ObjectLoaderSettings& settings)
    : verbose_(settings.verbose)
    , acceleration_context_(settings.acceleration_context) {
}

SceneObject ObjectLoader::LoadFromFile(const std::filesystem::path& file) {
    const aiScene* scene = importer_.ReadFile(file.string(), aiProcess_MakeLeftHanded | aiProcess_Triangulate);
    Ensure(
        scene != NULL && (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0 && scene->mRootNode != NULL,
        fmt::format("Failed to load object from file {}, reason:\n{}", file.string(), importer_.GetErrorString())
    );

    if (scene->HasAnimations()) {
        ReportLoadWarning(fmt::format("ignored {} animations", scene->mNumAnimations));
    }
    if (scene->HasCameras()) {
        ReportLoadWarning(fmt::format("ignored {} cameras", scene->mNumCameras));
    }
    if (scene->HasLights()) {
        ReportLoadWarning(fmt::format("ignored {} lights", scene->mNumLights));
    }

    textures_cache_.clear();
    materials_.clear();
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        materials_.emplace_back(LoadMaterial(file, scene, scene->mMaterials[i]));
    }

    meshes_.clear();
    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        auto& mesh = meshes_.emplace_back(LoadMesh(scene->mMeshes[i]));

        const auto material_id = scene->mMeshes[i]->mMaterialIndex;
        assert(material_id < materials_.size() && "Unexpected material id");
        mesh.SetMaterial(materials_[material_id]);
    }

    return BuildSceneObject(scene->mRootNode);
}

TextureView ObjectLoader::AddTexture(Texture::Ptr texture) {
    if (acceleration_context_) {
        texture->ToDevice(*acceleration_context_);
    }
    return TextureView(*textures_.emplace_back(std::move(texture)));
}

TextureView ObjectLoader::GetMonotonicTexture(aiColor3D color) {
    return AddTexture(Texture::Monotonic(Vec3(color.r, color.g, color.b)));
}

std::optional<TextureView> ObjectLoader::LoadTexture(
    const std::filesystem::path& file, const aiScene* scene, const aiMaterial* material, aiTextureType type
) {
    const auto testure_count = material->GetTextureCount(type);
    if (testure_count <= 0) {
        return std::nullopt;
    }
    if (testure_count > 1) {
        ReportLoadWarning(fmt::format(
            "material texture loading, ignored {} textures for type {}", testure_count - 1, static_cast<uint32_t>(type)
        ));
    }

    aiString texture_path;
    aiTextureMapping mapping = aiTextureMapping_UV;
    unsigned int uv_index = 0;
    if (material->GetTexture(type, 0, &texture_path, &mapping, &uv_index) != aiReturn_SUCCESS) {
        ReportLoadWarning(
            fmt::format("material texture loading failed for texture with type {}", static_cast<uint32_t>(type))
        );
        return std::nullopt;
    }

    if (mapping != aiTextureMapping_UV) {
        ReportLoadWarning(fmt::format(
            "material texture loading failed for texture with type {}, unsupported mapping {}",
            static_cast<uint32_t>(type), static_cast<uint32_t>(mapping)
        ));
        return std::nullopt;
    }

    if (uv_index > 0) {
        ReportLoadWarning(fmt::format(
            "material texture loading failed for texture with type {}, not first UV channel {}",
            static_cast<uint32_t>(type), uv_index
        ));
        return std::nullopt;
    }

    const std::string path(texture_path.data);
    if (path[0] == '*') {
        ReportLoadWarning(fmt::format(
            "material texture loading failed for texture with type {}, embedded textures is not supported",
            static_cast<uint32_t>(type)
        ));
        return std::nullopt;
    }

    if (const auto it = textures_cache_.find(path); it != textures_cache_.end()) {
        return TextureView(*textures_[it->second]);
    }

    textures_cache_[path] = textures_.size();
    return AddTexture(Texture::LoadFromFile(file.parent_path() / path));
}

Material ObjectLoader::LoadMaterial(
    const std::filesystem::path& file, const aiScene* scene, const aiMaterial* material
) {
    Material result;

    if (aiColor3D color; material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
        result.diffuse_tex = GetMonotonicTexture(color);
    }
    if (const auto tex = LoadTexture(file, scene, material, aiTextureType_DIFFUSE)) {
        if (result.diffuse_tex) {
            ReportLoadWarning("material loading, overrided monotonic diffuse color via texture");
        }
        result.diffuse_tex = tex;
    }

    if (aiColor3D color; material->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
        result.specular_tex = GetMonotonicTexture(color);
    }
    if (const auto tex = LoadTexture(file, scene, material, aiTextureType_SPECULAR)) {
        if (result.specular_tex) {
            ReportLoadWarning("material loading, overrided monotonic specular color via texture");
        }
        result.specular_tex = tex;
    }

    if (aiColor3D color; material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == aiReturn_SUCCESS) {
        result.emission_tex = GetMonotonicTexture(color);
    }
    if (const auto tex = LoadTexture(file, scene, material, aiTextureType_EMISSIVE)) {
        if (result.emission_tex) {
            ReportLoadWarning("material loading, overrided monotonic emission color via texture");
        }
        result.emission_tex = tex;
    }

    float shininess = 0.0;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
        result.shininess = shininess;
    }

    return result;
}

VerticesObject ObjectLoader::LoadMesh(const aiMesh* mesh) const {
    VerticesObject object(mesh->mNumVertices, VerticesObject::Type::Triangles);
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) == 0) {
        ReportLoadWarning("mesh loading failed, non triangle primitive type is not supported");
        return object;
    }

    if (mesh->HasBones()) {
        ReportLoadWarning(fmt::format("mesh loading, ignored {} bones", mesh->mNumBones));
    }
    if (mesh->HasTangentsAndBitangents()) {
        ReportLoadWarning(fmt::format("mesh loading, ignored tangents / bitangents"));
    }

    std::vector<uint64_t> indices;
    indices.reserve(3 * mesh->mNumFaces);
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
        const auto& face = mesh->mFaces[i];
        const auto num_indices = face.mNumIndices;
        assert(num_indices == 3 && "Unexpected number of indices for triangle face");

        for (uint32_t j = 0; j < num_indices; ++j) {
            indices.emplace_back(face.mIndices[j]);
        }
    }
    object.SetIndices(indices);

    std::vector<Vertex> vertices(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        vertices[i].position = VectorFromAssimp(mesh->mVertices[i]);
    }

    if (mesh->HasNormals()) {
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            vertices[i].params.normal = VectorFromAssimp(mesh->mNormals[i]);
        }
    }

    if (const auto count_chennels = mesh->GetNumUVChannels(); count_chennels > 0) {
        if (count_chennels > 1) {
            ReportLoadWarning(fmt::format("mesh texture coords loading, ignored {} UV channels", count_chennels - 1));
        }
        if (mesh->mNumUVComponents[0] == 2) {
            for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
                const auto& coords = mesh->mTextureCoords[0][i];
                vertices[i].params.tex_coords = Vec2(coords.x, coords.y);
            }
        } else {
            ReportLoadWarning("mesh texture coords loading failed, can not load not 2D textures");
        }
    }

    if (const auto count_chennels = mesh->GetNumColorChannels(); count_chennels > 0) {
        if (count_chennels > 1) {
            ReportLoadWarning(fmt::format("mesh colors loading, ignored {} color channels", count_chennels - 1));
        }
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            const auto& color = mesh->mColors[0][i];
            if (!Equal(color.a, 1.0)) {
                ReportLoadWarning("mesh colors loading, ignored alpha component");
            }
            vertices[i].params.color = Vec3(color.r, color.g, color.b);
        }
    }

    object.SetVertices(vertices);

    return object;
}

SceneObject ObjectLoader::BuildSceneObject(const aiNode* node) const {
    SceneObject object(TransformFromAssimp(node->mTransformation));
    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        const auto mesh_id = node->mMeshes[i];
        assert(mesh_id < meshes_.size() && "Unexpected mesh id");
        object.AddObject(meshes_[mesh_id]);
    }

    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        object.AddChild(BuildSceneObject(node->mChildren[i]));
    }

    return object;
}

void ObjectLoader::ReportLoadWarning(const std::string& text) const {
    if (verbose_) {
        std::cerr << "WARNING: " << text << "\n\n";
    }
}

}  // namespace null_engine
