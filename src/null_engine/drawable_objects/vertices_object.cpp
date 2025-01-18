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

        case Type::Lines:
            FillDefaultIndices(number_vertices - number_vertices % 2);
            break;

        case Type::LineStrip:
        case Type::LineLoop:
            if (number_vertices >= 2) {
                FillDefaultIndices(number_vertices);
            }
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

const std::vector<detail::LineIndex>& VerticesObject::GetLinesIndices() const {
    assert(IsLinesObject() && "Invalid object type for line indices");

    return line_indices_;
}

const std::vector<detail::TriangleIndex>& VerticesObject::GetTriangleIndices() const {
    assert(IsTrianglesObject() && "Invalid object type for line indices");

    return triangle_indices_;
}

bool VerticesObject::IsPointsObject() const {
    return object_type_ == Type::Points;
}

bool VerticesObject::IsLinesObject() const {
    return object_type_ == Type::Lines || object_type_ == Type::LineStrip || object_type_ == Type::LineLoop;
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

    line_indices_.clear();
    triangle_indices_.clear();
    switch (object_type_) {
        case Type::Lines:
            assert(indices.size() % 2 == 0 && "Invalid number of indices for lines object type");

            line_indices_.reserve(indices.size() / 2);
            for (size_t i = 0; i < indices.size(); i += 2) {
                line_indices_.emplace_back(indices[i], indices[i + 1]);
            }
            break;

        case Type::LineStrip:
            assert(indices.size() >= 2 && "Lines strip should contain at least two indices");

            line_indices_.reserve(indices.size() - 1);
            for (size_t i = 1; i < indices.size(); ++i) {
                line_indices_.emplace_back(indices[i - 1], indices[i]);
            }
            break;

        case Type::LineLoop:
            assert(indices.size() >= 2 && "Lines loop should contain at least two indices");

            line_indices_.reserve(indices.size());
            for (size_t i = 1; i < indices.size(); ++i) {
                line_indices_.emplace_back(indices[i - 1], indices[i]);
            }
            line_indices_.emplace_back(indices.back(), indices[0]);
            break;

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

    return *this;
}

VerticesObject& VerticesObject::Transform(const Mat4& transform) {
    for (auto& [position, _] : vertices_) {
        position = transform.Apply(position).XYZ();
    }
    return *this;
}

VerticesObject& VerticesObject::GenerateNormals(bool clockwise) {
    assert(IsTrianglesObject() && "Can not generate normals for not triangles object");

    std::vector<size_t> traingles_per_vertex(vertices_.size());

    const auto add_normal = [&](uint64_t id_a, const Vec3& point_b, const Vec3& point_c) {
        const auto& point_a = vertices_[id_a].position;

        auto normal = (point_b - point_a).VectorProd(point_c - point_a);
        if (!normal.IsZero()) {
            normal.Normalize();
        }
        vertices_[id_a].params.normal += clockwise ? normal : -normal;

        ++traingles_per_vertex[id_a];
    };

    for (const auto& [id_a, id_b, id_c] : GetTriangleIndices()) {
        add_normal(id_a, vertices_[id_b].position, vertices_[id_c].position);
        add_normal(id_b, vertices_[id_c].position, vertices_[id_a].position);
        add_normal(id_c, vertices_[id_a].position, vertices_[id_b].position);
    }

    for (size_t i = 0; i < vertices_.size(); ++i) {
        auto& normal = vertices_[i].params.normal;
        if (const auto denom = traingles_per_vertex[i]) {
            normal /= denom;
        }
        if (!normal.IsZero()) {
            normal.Normalize();
        }
    }

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

}  // namespace null_engine
