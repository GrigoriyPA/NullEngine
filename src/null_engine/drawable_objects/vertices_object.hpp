#pragma once

#include <null_engine/drawable_objects/common/vertex.hpp>
#include <vector>

namespace null_engine {

namespace detail {

struct TriangleIndex {
    uint64_t point_a;
    uint64_t point_b;
    uint64_t point_c;
};

}  // namespace detail

class VerticesObject {
    using TriangleIndex = detail::TriangleIndex;

public:
    enum class Type { Points, Triangles, TriangleStrip, TriangleFan };

    explicit VerticesObject(uint64_t number_vertices, Type object_type = Type::Points);

    Type GetObjectType() const;

    size_t GetNumberVertices() const;

    const std::vector<Vertex>& GetVertices() const;

    const std::vector<uint64_t>& GetIndices() const;

    const std::vector<TriangleIndex>& GetTriangleIndices() const;

    bool IsPointsObject() const;

    bool IsTrianglesObject() const;

    VerticesObject& SetVertex(uint64_t index, const Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<Vec3>& positions);

    VerticesObject& SetParams(const std::vector<VertexParams>& params);

    VerticesObject& SetParams(const VertexParams& params);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

private:
    bool ValidateIdicesValues(const std::vector<uint64_t>& indices) const;

    void FillDefaultIndices(size_t indices_size);

    void FillTriangleIndices(const std::vector<uint64_t>& indices);

    Type object_type_;
    std::vector<Vertex> vertices_;
    std::vector<uint64_t> indices_;
    std::vector<TriangleIndex> triangle_indices_;
};

}  // namespace null_engine
