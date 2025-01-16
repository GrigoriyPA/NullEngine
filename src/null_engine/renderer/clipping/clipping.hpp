#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>

namespace null_engine::detail {

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
    static TriangleClippingResult ClipTriangles(std::vector<Vertex> vertices, std::vector<TriangleIndex> indices);

private:
    static void ClipTrianglesByPlane(TriangleClippingResult& triangles, Vec4 normal);

    static void ClipTriangle(
        TriangleClippingResult& triangles, ClippingPoint point_a, ClippingPoint point_b, ClippingPoint point_c
    );

    static void AddInterpolatedPoint(TriangleClippingResult& triangles, ClippingPoint inside, ClippingPoint outside);
};

}  // namespace null_engine::detail
