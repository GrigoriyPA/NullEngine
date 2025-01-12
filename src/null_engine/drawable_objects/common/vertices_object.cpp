#include "vertices_object.hpp"

#include <fmt/core.h>

#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::drawable {

VerticesObject::VerticesObject(uint64_t number_vertices)
    : vertices_(number_vertices, generic::Vertex(util::Vec3(0.0), util::Vec3(1.0)))
    , indices_(number_vertices) {
    for (size_t i = 0; i < indices_.size(); ++i) {
        indices_[i] = i;
    }
}

const std::vector<generic::Vertex>& VerticesObject::GetVertices() const {
    return vertices_;
}

const std::vector<uint64_t>& VerticesObject::GetIndices() const {
    return indices_;
}

VerticesObject& VerticesObject::SetVertex(uint64_t index, const generic::Vertex& vertex) {
    util::Ensure(
        index < vertices_.size(),
        fmt::format("Invalid vertex index {} for vertex object with {} vertices", index, vertices_.size())
    );

    vertices_[index] = vertex;
    return *this;
}

VerticesObject& VerticesObject::SetPositions(const std::vector<util::Vec3>& positions) {
    util::Ensure(
        positions.size() == vertices_.size(),
        fmt::format("Invalid positions size {} for vertex object with {} vertices", positions.size(), vertices_.size())
    );

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetPosition(positions[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColors(const std::vector<util::Vec3>& colors) {
    util::Ensure(
        colors.size() == vertices_.size(),
        fmt::format("Invalid colors size for vertex object with vertices", colors.size(), vertices_.size())
    );

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetColor(colors[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColor(util::Vec3 color) {
    for (auto& vertex : vertices_) {
        vertex.SetColor(color);
    }
    return *this;
}

VerticesObject& VerticesObject::SetIndices(const std::vector<uint64_t>& indices) {
    for (const auto index : indices) {
        util::Ensure(
            index < vertices_.size(),
            fmt::format("Invalid index {} for vertex object with {} vertices", index, vertices_.size())
        );
    }

    indices_ = indices;
    return *this;
}

}  // namespace null_engine::drawable
