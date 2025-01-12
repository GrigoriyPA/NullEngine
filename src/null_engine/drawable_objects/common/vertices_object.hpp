#pragma once

#include <cstdint>
#include <null_engine/generic/mesh/generic_mesh.hpp>
#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/util/generic/templates.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>
#include <vector>

namespace null_engine::drawable {

//
// Generic drawable object,
// it contains vertex array, single material
// and vertex attributes such as texture coordinates
//
class VerticesObject : public generic::MeshObject, public util::SharedConstructable<VerticesObject> {
public:
    using Ptr = std::shared_ptr<VerticesObject>;

public:
    explicit VerticesObject(uint64_t number_vertices);

    VerticesObject& SetVertex(uint64_t index, const generic::Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<util::Vec3>& positions);

    VerticesObject& SetColors(const std::vector<util::Vec3>& colors);
    VerticesObject& SetColor(util::Vec3 color);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

public:
    const std::vector<generic::Vertex>& GetVertices() const override;
    const std::vector<uint64_t>& GetIndices() const override;

private:
    std::vector<generic::Vertex> vertices_;
    std::vector<uint64_t> indices_;
};

namespace tests {

VerticesObject::Ptr CreatePointsSet(
    uint64_t number_points, util::Vec3 offset = util::Vec3(-0.5, -0.5), util::Vec3 size = util::Vec3(1.0, 1.0),
    util::Vec3 color = util::Vec3(1.0)
);

}  // namespace tests

}  // namespace null_engine::drawable
