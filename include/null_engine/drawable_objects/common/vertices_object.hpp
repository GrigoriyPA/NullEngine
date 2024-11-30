#pragma once

#include <null_engine/generic/mesh/generic_mesh.hpp>

#include <null_engine/native/common/native_vertex.hpp>

#include <null_engine/util/geometry/vector_3d.hpp>

#include <cstdint>
#include <vector>

namespace null_engine::drawable {

//
// Generic drawable object,
// it contains vertex array, single material
// and vertex attributes such as texture coordinates
//
class VerticesObject : public generic::MeshObject {
public:
    using Ptr = std::shared_ptr<VerticesObject>;

public:
    explicit VerticesObject(uint64_t number_vertices);

    VerticesObject& SetPositions(const std::vector<util::Vec3>& positions);

    VerticesObject& SetColors(const std::vector<util::Vec3>& colors);
    VerticesObject& SetColor(util::Vec3 color);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

public:
    template <typename... Args>
    static VerticesObject::Ptr Make(Args&&... args) {
        return std::make_shared<VerticesObject>(std::forward<Args>(args)...);
    }

private:
    std::vector<native::Vertex> vertices_;
    std::vector<uint64_t> indices_;  // Indices in vertex array
};

}  // namespace null_engine::drawable
