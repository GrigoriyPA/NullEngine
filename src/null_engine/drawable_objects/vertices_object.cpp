#include "vertices_object.hpp"

#include <cassert>

namespace null_engine {

VerticesObject::VerticesObject(uint64_t number_vertices)
    : vertices_(number_vertices, Vertex(Vec3(0.0), Vec3(1.0)))
    , indices_(number_vertices) {
    for (size_t i = 0; i < indices_.size(); ++i) {
        indices_[i] = i;
    }
}

const std::vector<Vertex>& VerticesObject::GetVertices() const {
    return vertices_;
}

const std::vector<uint64_t>& VerticesObject::GetIndices() const {
    return indices_;
}

VerticesObject& VerticesObject::SetVertex(uint64_t index, const Vertex& vertex) {
    assert(index < vertices_.size() && "Vertex index too large");

    vertices_[index] = vertex;
    return *this;
}

VerticesObject& VerticesObject::SetPositions(const std::vector<Vec3>& positions) {
    assert(positions.size() == vertices_.size() && "Number of positions and verticies should be equal");

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetPosition(positions[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColors(const std::vector<Vec3>& colors) {
    assert(colors.size() == vertices_.size() && "Number of colors and verticies should be equal");

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetColor(colors[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColor(Vec3 color) {
    for (auto& vertex : vertices_) {
        vertex.SetColor(color);
    }
    return *this;
}

VerticesObject& VerticesObject::SetIndices(const std::vector<uint64_t>& indices) {
    assert(ValidateIdices(indices) && "Vertex index too large");

    indices_ = indices;
    return *this;
}

bool VerticesObject::ValidateIdices(const std::vector<uint64_t>& indices) const {
    for (const auto index : indices) {
        if (index >= vertices_.size()) {
            return false;
        }
    }
    return true;
}

}  // namespace null_engine
