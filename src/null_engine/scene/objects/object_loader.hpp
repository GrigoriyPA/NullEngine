#pragma once

#include <assimp/material.h>
#include <assimp/mesh.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/drawable_objects/material/texture.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/geometry/matrix.hpp>

#include "scene_object.hpp"

namespace null_engine {

struct ObjectLoaderSettings {
    bool verbose = true;
    std::optional<multithread::AccelerationContext> acceleration_context;
};

class ObjectLoader {
    using AccelerationContext = multithread::AccelerationContext;

public:
    explicit ObjectLoader(const ObjectLoaderSettings& settings);

    SceneObject LoadFromFile(const std::filesystem::path& file);

private:
    TextureView AddTexture(Texture::Ptr texture);

    TextureView GetMonotonicTexture(aiColor3D color);

    std::optional<TextureView> LoadTexture(
        const std::filesystem::path& file, const aiScene* scene, const aiMaterial* material, aiTextureType type
    );

    Material LoadMaterial(const std::filesystem::path& file, const aiScene* scene, const aiMaterial* material);

    VerticesObject LoadMesh(const aiMesh* mesh) const;

    SceneObject BuildSceneObject(const aiNode* node) const;

    void ReportLoadWarning(const std::string& text) const;

    bool verbose_ = true;
    Assimp::Importer importer_;
    std::vector<Material> materials_;
    std::vector<VerticesObject> meshes_;
    std::vector<Texture::Ptr> textures_;
    std::unordered_map<std::string, size_t> textures_cache_;
    std::optional<AccelerationContext> acceleration_context_;
};

}  // namespace null_engine
