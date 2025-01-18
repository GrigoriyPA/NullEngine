#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/renderer/shaders/vertex_shader.hpp>

namespace null_engine {

struct ClipperSettings {
    bool clip_triangles_by_normals = true;
};

namespace detail {

struct LineClippingResult {
    std::vector<InterpVertex> vertices;
    std::vector<LineIndex> indices;
};

struct TriangleClippingResult {
    std::vector<InterpVertex> vertices;
    std::vector<TriangleIndex> indices;
};

class Clipper {
    struct ClippingPoint {
        FloatType scalar_prod;
        uint64_t index;
    };

public:
    explicit Clipper(const ClipperSettings& settings);

    LineClippingResult ClipLines(std::vector<InterpVertex> vertices, std::vector<LineIndex> indices);

    TriangleClippingResult ClipTriangles(
        Vec3 view_pos, std::vector<InterpVertex> vertices, std::vector<TriangleIndex> indices
    );

private:
    void ClipLinesByPlane(Vec4 plane_normal);

    void ClipTrianglesByPlane(Vec4 plane_normal);

    void ClipLine(ClippingPoint point_a, ClippingPoint point_b);

    void ClipTriangle(ClippingPoint point_a, ClippingPoint point_b, ClippingPoint point_c);

    void AddInterpolatedPoint(ClippingPoint inside, ClippingPoint outside);

    void ClipTrianglesByNormals(Vec3 view_pos);

    ClipperSettings settings_;
    std::vector<Vec4> clipping_planes_;
    std::vector<InterpVertex> vertices_;
    std::vector<LineIndex> line_indices_;
    std::vector<TriangleIndex> triangle_indices_;
};

}  // namespace detail

}  // namespace null_engine
