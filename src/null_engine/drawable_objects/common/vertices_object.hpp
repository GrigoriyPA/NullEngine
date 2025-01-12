#pragma once

#include <cstdint>
#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>
#include <vector>

namespace null_engine::drawable {

class VerticesObject {
public:
    explicit VerticesObject(uint64_t number_vertices);

    const std::vector<generic::Vertex>& GetVertices() const;

    const std::vector<uint64_t>& GetIndices() const;

    VerticesObject& SetVertex(uint64_t index, const generic::Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<util::Vec3>& positions);

    VerticesObject& SetColors(const std::vector<util::Vec3>& colors);

    VerticesObject& SetColor(util::Vec3 color);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

private:
    std::vector<generic::Vertex> vertices_;
    std::vector<uint64_t> indices_;
};

}  // namespace null_engine::drawable
