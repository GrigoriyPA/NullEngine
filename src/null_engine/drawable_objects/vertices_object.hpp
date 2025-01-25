#pragma once

#include <null_engine/drawable_objects/common/vertex.hpp>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/util/geometry/matrix.hpp>

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

    const Material& GetMaterial() const;

    size_t GetNumberVertices() const;

    size_t GetNumberIndices() const;

    const std::vector<Vertex>& GetVertices() const;

    const std::vector<uint64_t>& GetIndices() const;

    const std::vector<LineIndex>& GetLinesIndices() const;

    const std::vector<TriangleIndex>& GetTriangleIndices() const;

    bool IsPointsObject() const;

    bool IsLinesObject() const;

    bool IsTrianglesObject() const;

    VerticesObject& SetMaterial(const Material& material);

    VerticesObject& SetVertex(uint64_t index, const Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<Vec3>& positions);

    VerticesObject& SetColors(const std::vector<Vec3>& colors);

    VerticesObject& SetColors(Vec3 color);

    VerticesObject& SetNormals(const std::vector<Vec3>& normals);

    VerticesObject& SetTexCoords(const std::vector<Vec2>& tex_coords);

    VerticesObject& SetParams(const std::vector<VertexParams>& params);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

    VerticesObject& ApplyTransform(const Transform& transform);

    VerticesObject& GenerateNormals(bool clockwise = true);

    VerticesObject& Merge(const VerticesObject& other);

private:
    bool ValidateIdicesValues(const std::vector<uint64_t>& indices) const;

    void FillDefaultIndices(size_t indices_size);

    Type object_type_;
    Material material_;
    std::vector<Vertex> vertices_;
    std::vector<uint64_t> indices_;
    std::vector<LineIndex> line_indices_;
    std::vector<TriangleIndex> triangle_indices_;
};

}  // namespace null_engine
