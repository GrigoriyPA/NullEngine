#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>

namespace null_engine::detail {

struct LineClippingResult {
    std::vector<Vertex> vertices;
    std::vector<LineIndex> indices;
};

struct TriangleClippingResult {
    std::vector<Vertex> vertices;
    std::vector<TriangleIndex> indices;
};

class Clipper {
    struct ClippingPoint {
        FloatType scalar_prod;
        uint64_t index;
    };

public:
    Clipper();

    LineClippingResult ClipLines(std::vector<Vertex> vertices, std::vector<LineIndex> indices);

    TriangleClippingResult ClipTriangles(std::vector<Vertex> vertices, std::vector<TriangleIndex> indices);

private:
    void ClipLinesByPlane(Vec4 plane_normal);

    void ClipTrianglesByPlane(Vec4 plane_normal);

    void ClipLine(ClippingPoint point_a, ClippingPoint point_b);

    void ClipTriangle(ClippingPoint point_a, ClippingPoint point_b, ClippingPoint point_c);

    void AddInterpolatedPoint(ClippingPoint inside, ClippingPoint outside);

    std::vector<Vec4> clipping_planes_;
    std::vector<Vertex> vertices_;
    std::vector<LineIndex> line_indices_;
    std::vector<TriangleIndex> triangle_indices_;
};

}  // namespace null_engine::detail
