#pragma once

#include <assimp/material.h>
#include <assimp/mesh.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/geometry/matrix.hpp>

#include "scene_object.hpp"

namespace null_engine {

struct ObjectLoaderSettings {
    bool verbose = true;
};

class ObjectLoader {
public:
    explicit ObjectLoader(const ObjectLoaderSettings& settings);

    SceneObject LoadFromFile(const std::filesystem::path& file, const Transform& instance);

private:
    Material LoadMaterial(const std::filesystem::path& file, const aiScene* scene, const aiMaterial* material);

    VerticesObject LoadMesh(const aiMesh* mesh) const;

    void BuildSceneObject(SceneObject& object, const aiNode* node, Transform transform) const;

    void ReportLoadWarning(const std::string& text) const;

    bool verbose_ = true;
    Assimp::Importer importer_;
    std::vector<Material> materials_;
    std::vector<VerticesObject> meshes_;
    std::unordered_map<std::string, Texture> testures_;
};

}  // namespace null_engine
