#include "clipping.hpp"

#include <cassert>

namespace null_engine::detail {

namespace {

void RemoveTriange(std::vector<TriangleIndex>& indices, int64_t i) {
    assert(i < indices.size() && "Invalid index for remove");

    if (i + 1 < indices.size()) {
        std::swap(indices[i], indices.back());
    }
    indices.pop_back();
}

}  // anonymous namespace

TriangleClippingResult Clipper::ClipTriangles(std::vector<Vertex> vertices, std::vector<TriangleIndex> indices) {
    TriangleClippingResult result{.vertices = std::move(vertices), .indices = std::move(indices)};

    ClipTrianglesByPlane(result, Vec4(0.0, 0.0, -1.0, 1.0));
    ClipTrianglesByPlane(result, Vec4(0.0, 0.0, 1.0, 1.0));

    ClipTrianglesByPlane(result, Vec4(-1.0, 0.0, 0.0, 1.0));
    ClipTrianglesByPlane(result, Vec4(1.0, 0.0, 0.0, 1.0));

    ClipTrianglesByPlane(result, Vec4(0.0, -1.0, 0.0, 1.0));
    ClipTrianglesByPlane(result, Vec4(0.0, 1.0, 0.0, 1.0));

    return result;
}

void Clipper::ClipTrianglesByPlane(TriangleClippingResult& triangles, Vec4 normal) {
    auto& indices = triangles.indices;
    if (indices.empty()) {
        return;
    }

    auto& vertices = triangles.vertices;
    for (int64_t i = indices.size() - 1; i >= 0; --i) {
        const auto [id_a, id_b, id_c] = indices[i];

        auto prod_a = normal.ScalarProd(vertices[id_a].position);
        auto prod_b = normal.ScalarProd(vertices[id_b].position);
        auto prod_c = normal.ScalarProd(vertices[id_c].position);

        const uint32_t number_inside = static_cast<uint32_t>(prod_a >= -kEps) + static_cast<uint32_t>(prod_b >= -kEps) +
                                       static_cast<uint32_t>(prod_c >= -kEps);
        if (number_inside == 3) {
            continue;
        }

        RemoveTriange(indices, i);

        const uint32_t number_outside = static_cast<uint32_t>(prod_a < kEps) + static_cast<uint32_t>(prod_b < kEps) +
                                        static_cast<uint32_t>(prod_c < kEps);
        if (number_outside == 3) {
            continue;
        }

        ClipTriangle(
            triangles, {.scalar_prod = prod_a, .index = id_a}, {.scalar_prod = prod_b, .index = id_b},
            {.scalar_prod = prod_c, .index = id_c}
        );
    }
}

void Clipper::ClipTriangle(
    TriangleClippingResult& triangles, ClippingPoint point_a, ClippingPoint point_b, ClippingPoint point_c
) {
    if (point_a.scalar_prod < kEps) {
        std::swap(point_a, point_c);
        if (point_a.scalar_prod < kEps) {
            std::swap(point_a, point_b);
        }
    }
    if (point_b.scalar_prod < -kEps) {
        std::swap(point_b, point_c);
    }
    assert(
        point_a.scalar_prod >= kEps && "Expected at least one point with positive scalar production with clip plane"
    );

    auto& indices = triangles.indices;
    auto& vertices = triangles.vertices;
    if (point_b.scalar_prod < -kEps) {
        AddInterpolatedPoint(triangles, point_a, point_b);
        AddInterpolatedPoint(triangles, point_a, point_c);

        indices.emplace_back(point_a.index, vertices.size() - 2, vertices.size() - 1);
    } else if (point_c.scalar_prod < -kEps) {
        AddInterpolatedPoint(triangles, point_a, point_c);
        AddInterpolatedPoint(triangles, point_b, point_c);

        indices.emplace_back(point_a.index, point_b.index, vertices.size() - 1);
        indices.emplace_back(point_a.index, vertices.size() - 2, vertices.size() - 1);
    } else {
        assert(false && "Expected at least one point with negative scalar production with clip plane");
    }
}

void Clipper::AddInterpolatedPoint(TriangleClippingResult& triangles, ClippingPoint inside, ClippingPoint outside) {
    assert(inside.scalar_prod >= -kEps && "First point should be inside clip plane");
    assert(outside.scalar_prod < kEps && "Second point should be outside clip plane");

    auto vertex_a = triangles.vertices[inside.index];
    auto vertex_b = triangles.vertices[outside.index];

    const auto delta = inside.scalar_prod - outside.scalar_prod;
    if (delta < kEps) {
        vertex_a += vertex_b;
        vertex_a *= 0.5;
        triangles.vertices.emplace_back(std::move(vertex_a));
        return;
    }

    const auto ratio = inside.scalar_prod / delta;
    vertex_a *= 1.0 - ratio + kEps;
    vertex_b *= ratio - kEps;

    vertex_a += vertex_b;
    triangles.vertices.emplace_back(std::move(vertex_a));
}

}  // namespace null_engine::detail
