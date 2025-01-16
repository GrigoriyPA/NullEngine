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

        case Type::TriangleStrip:
        case Type::TriangleFan:
            if (number_vertices >= 3) {
                FillDefaultIndices(number_vertices);
            }
            break;
    }
}

VerticesObject::Type VerticesObject::GetObjectType() const {
    return object_type_;
}

size_t VerticesObject::GetNumberVertices() const {
    return vertices_.size();
}

const std::vector<Vertex>& VerticesObject::GetVertices() const {
    return vertices_;
}

const std::vector<uint64_t>& VerticesObject::GetIndices() const {
    return indices_;
}

const std::vector<detail::TriangleIndex>& VerticesObject::GetTriangleIndices() const {
    return triangle_indices_;
}

bool VerticesObject::IsPointsObject() const {
    return object_type_ == Type::Points;
}

bool VerticesObject::IsTrianglesObject() const {
    return object_type_ == Type::Triangles || object_type_ == Type::TriangleStrip || object_type_ == Type::TriangleFan;
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

    indices_ = indices;
    FillTriangleIndices(indices);

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
    std::vector<uint64_t> indices(indices_size);
    for (size_t i = 0; i < indices_size; ++i) {
        indices[i] = i;
    }
    SetIndices(indices);
}

void VerticesObject::FillTriangleIndices(const std::vector<uint64_t>& indices) {
    triangle_indices_.clear();
    switch (object_type_) {
        case Type::Triangles:
            assert(indices.size() % 3 == 0 && "Invalid number of indices for triangles object type");

            triangle_indices_.reserve(indices.size() / 3);
            for (size_t i = 0; i < indices.size(); i += 3) {
                triangle_indices_.emplace_back(indices[i], indices[i + 1], indices[i + 2]);
            }
            break;

        case Type::TriangleStrip:
            assert(indices.size() >= 3 && "Triangles strip should contain at least three indices");

            triangle_indices_.reserve(indices.size() - 2);
            for (size_t i = 2; i < indices.size(); ++i) {
                triangle_indices_.emplace_back(indices[i - 2], indices[i - 1], indices[i]);
            }
            break;

        case Type::TriangleFan:
            assert(indices.size() >= 3 && "Triangles fan should contain at least three indices");

            triangle_indices_.reserve(indices.size() - 2);
            for (size_t i = 2; i < indices.size(); ++i) {
                triangle_indices_.emplace_back(indices[0], indices[i - 1], indices[i]);
            }
            break;

        default:
            break;
    }
}

}  // namespace null_engine
