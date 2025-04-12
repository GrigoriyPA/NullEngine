#include "object_loader.hpp"

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/core.h>

#include <cstddef>
#include <iostream>
#include <null_engine/drawable_objects/common/vertex.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <null_engine/util/geometry/matrix.hpp>

namespace null_engine {

ObjectLoader::ObjectLoader(const ObjectLoaderSettings& settings)
    : verbose_(settings.verbose) {
}

SceneObject ObjectLoader::LoadFromFile(const std::filesystem::path& file, const Transform& instance) {
    const aiScene* scene = importer_.ReadFile(file.string(), aiProcess_MakeLeftHanded | aiProcess_Triangulate);
    Ensure(
        scene != NULL && (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0 && scene->mRootNode != NULL,
        fmt::format("Failed to load object from file {}, reason:\n{}", file.string(), importer_.GetErrorString())
    );

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

    SceneObject root_object(instance);
    BuildSceneObject(root_object, scene->mRootNode, Ident());
    return root_object;
}

Material ObjectLoader::LoadMaterial(
    const std::filesystem::path& file, const aiScene* scene, const aiMaterial* material
) {
    Material result;

    float shininess = 0.0;
    if (material->Get(AI_MATKEY_OPACITY, shininess) == aiReturn_SUCCESS) {
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

    if (mesh->GetNumUVChannels() == 1) {
        if (mesh->mNumUVComponents[0] == 2) {
            for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
                const auto& coords = mesh->mTextureCoords[0][i];
                vertices[i].params.tex_coords = Vec2(coords.x, coords.y);
            }
        } else {
            ReportLoadWarning("mesh testure coords loading failed, can not load not 2D textures");
        }
    } else if (mesh->GetNumUVChannels() > 0) {
        ReportLoadWarning("mesh testure coords loading failed, can not load more than one channel");
    }

    if (mesh->GetNumColorChannels() == 1) {
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            const auto& color = mesh->mColors[0][i];
            if (!Equal(color.a, 1.0)) {
                ReportLoadWarning("mesh colors loading failed, can not load alpha component");
                break;
            }

            vertices[i].params.color = Vec3(color.r, color.g, color.b);
        }
    } else if (mesh->GetNumColorChannels() > 0) {
        ReportLoadWarning("mesh colors loading failed, can not load more than one channel");
    }

    object.SetVertices(vertices);

    return object;
}

void ObjectLoader::BuildSceneObject(SceneObject& object, const aiNode* node, Transform transform) const {
    transform = transform * TransformFromAssimp(node->mTransformation);

    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        const auto mesh_id = node->mMeshes[i];
        assert(mesh_id < meshes_.size() && "Unexpected mesh id");
        object.EmplaceChild(meshes_[mesh_id], transform);
    }

    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        BuildSceneObject(object, node->mChildren[i], transform);
    }
}

void ObjectLoader::ReportLoadWarning(const std::string& text) const {
    if (verbose_) {
        std::cerr << "WARNING: " << text << "\n\n";
    }
}

}  // namespace null_engine
