#include "vertices_object.hpp"

#include <cassert>

namespace null_engine {

VerticesObject::VerticesObject(uint64_t number_vertices, Type object_type)
    : vertices_(number_vertices)
    , object_type_(object_type) {
    switch (object_type_) {
        case Type::Points:
            FillDefaultIndices(number_vertices);
            break;

        case Type::Triangles:
            FillDefaultIndices(number_vertices - number_vertices % 3);
            break;
    }
}

VerticesObject::Type VerticesObject::GetObjectType() const {
    return object_type_;
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
        vertices_[i].position = positions[i];
    }
    return *this;
}

VerticesObject& VerticesObject::SetParams(const std::vector<VertexParams>& params) {
    assert(params.size() == vertices_.size() && "Number of params and verticies should be equal");

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].params = params[i];
    }
    return *this;
}

VerticesObject& VerticesObject::SetParams(const VertexParams& params) {
    for (auto& vertex : vertices_) {
        vertex.params = params;
    }
    return *this;
}

VerticesObject& VerticesObject::SetIndices(const std::vector<uint64_t>& indices) {
    assert(ValidateIdicesValues(indices) && "Vertex index too large");

    if (object_type_ == Type::Triangles) {
        assert(indices.size() % 3 == 0 && "Invalid number of indices for triangles object type");
    }

    indices_ = indices;
    return *this;
}

bool VerticesObject::ValidateIdicesValues(const std::vector<uint64_t>& indices) const {
    for (const auto index : indices) {
        if (index >= vertices_.size()) {
            return false;
        }
    }
    return true;
}

void VerticesObject::FillDefaultIndices(size_t indices_size) {
    indices_.resize(indices_size);
    for (size_t i = 0; i < indices_size; ++i) {
        indices_[i] = i;
    }
}

}  // namespace null_engine
