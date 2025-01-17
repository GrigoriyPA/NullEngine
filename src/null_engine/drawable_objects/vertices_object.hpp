#pragma once

#include <null_engine/drawable_objects/common/vertex.hpp>
#include <null_engine/util/geometry/matrix4.hpp>
#include <vector>

namespace null_engine {

namespace detail {

struct LineIndex {
    uint64_t point_a;
    uint64_t point_b;
};

struct TriangleIndex {
    uint64_t point_a;
    uint64_t point_b;
    uint64_t point_c;
};

}  // namespace detail

class VerticesObject {
    using LineIndex = detail::LineIndex;
    using TriangleIndex = detail::TriangleIndex;

public:
    enum class Type { Points, Lines, LineStrip, LineLoop, Triangles, TriangleStrip, TriangleFan };

    explicit VerticesObject(uint64_t number_vertices, Type object_type = Type::Points);

    Type GetObjectType() const;

    size_t GetNumberVertices() const;

    const std::vector<Vertex>& GetVertices() const;

    const std::vector<uint64_t>& GetIndices() const;

    const std::vector<LineIndex>& GetLinesIndices() const;

    const std::vector<TriangleIndex>& GetTriangleIndices() const;

    bool IsPointsObject() const;

    bool IsLinesObject() const;

    bool IsTrianglesObject() const;

    VerticesObject& SetVertex(uint64_t index, const Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<Vec3>& positions);

    VerticesObject& SetParams(const std::vector<VertexParams>& params);

    VerticesObject& SetParams(const VertexParams& params);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

    VerticesObject& Transform(const Mat4& transform);

private:
    bool ValidateIdicesValues(const std::vector<uint64_t>& indices) const;

    void FillDefaultIndices(size_t indices_size);

    Type object_type_;
    std::vector<Vertex> vertices_;
    std::vector<uint64_t> indices_;
    std::vector<LineIndex> line_indices_;
    std::vector<TriangleIndex> triangle_indices_;
};

}  // namespace null_engine
