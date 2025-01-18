#include "clipping.hpp"

#include <cassert>
#include <null_engine/util/generic/helpers.hpp>

namespace null_engine::detail {

namespace {

bool IsWisiblePoint(const InterpVertex& point, Vec3 view_pos) {
    const auto normal = point.params.normal;
    if (normal.IsZero()) {
        return true;
    }

    const auto direction = view_pos - point.params.frag_pos;
    return normal.ScalarProd(direction) > 0.0;
}

}  // anonymous namespace

Clipper::Clipper(const ClipperSettings& settings)
    : settings_(settings)
    , clipping_planes_({
          Vec4(0.0, 0.0, -1.0, 1.0),
          Vec4(0.0, 0.0, 1.0, 1.0),
          Vec4(-1.0, 0.0, 0.0, 1.0),
          Vec4(1.0, 0.0, 0.0, 1.0),
          Vec4(0.0, -1.0, 0.0, 1.0),
          Vec4(0.0, 1.0, 0.0, 1.0),
      }) {
}

LineClippingResult Clipper::ClipLines(std::vector<InterpVertex> vertices, std::vector<LineIndex> indices) {
    vertices_.swap(vertices);
    line_indices_.swap(indices);

    for (auto plane_normal : clipping_planes_) {
        ClipLinesByPlane(plane_normal);
    }

    LineClippingResult result;
    result.vertices.swap(vertices_);
    result.indices.swap(line_indices_);

    return result;
}

TriangleClippingResult Clipper::ClipTriangles(
    Vec3 view_pos, std::vector<InterpVertex> vertices, std::vector<TriangleIndex> indices
) {
    vertices_.swap(vertices);
    triangle_indices_.swap(indices);

    for (auto plane_normal : clipping_planes_) {
        ClipTrianglesByPlane(plane_normal);
    }
    if (settings_.clip_triangles_by_normals) {
        ClipTrianglesByNormals(view_pos);
    }

    TriangleClippingResult result;
    result.vertices.swap(vertices_);
    result.indices.swap(triangle_indices_);

    return result;
}

void Clipper::ClipLinesByPlane(Vec4 plane_normal) {
    if (line_indices_.empty()) {
        return;
    }

    for (int64_t i = line_indices_.size() - 1; i >= 0; --i) {
        const auto [id_a, id_b] = line_indices_[i];

        const auto prod_a = plane_normal.ScalarProd(vertices_[id_a].position);
        const auto prod_b = plane_normal.ScalarProd(vertices_[id_b].position);

        const uint32_t number_inside = static_cast<uint32_t>(prod_a >= -kEps) + static_cast<uint32_t>(prod_b >= -kEps);
        if (number_inside == 2) {
            continue;
        }

        SwapRemove(line_indices_, i);

        const uint32_t number_outside = static_cast<uint32_t>(prod_a < kEps) + static_cast<uint32_t>(prod_b < kEps);
        if (number_outside == 2) {
            continue;
        }

        ClipLine({.scalar_prod = prod_a, .index = id_a}, {.scalar_prod = prod_b, .index = id_b});
    }
}

void Clipper::ClipTrianglesByPlane(Vec4 plane_normal) {
    if (triangle_indices_.empty()) {
        return;
    }

    for (int64_t i = triangle_indices_.size() - 1; i >= 0; --i) {
        const auto [id_a, id_b, id_c] = triangle_indices_[i];

        const auto prod_a = plane_normal.ScalarProd(vertices_[id_a].position);
        const auto prod_b = plane_normal.ScalarProd(vertices_[id_b].position);
        const auto prod_c = plane_normal.ScalarProd(vertices_[id_c].position);

        const uint32_t number_inside = static_cast<uint32_t>(prod_a >= -kEps) + static_cast<uint32_t>(prod_b >= -kEps) +
                                       static_cast<uint32_t>(prod_c >= -kEps);
        if (number_inside == 3) {
            continue;
        }

        SwapRemove(triangle_indices_, i);

        const uint32_t number_outside = static_cast<uint32_t>(prod_a < kEps) + static_cast<uint32_t>(prod_b < kEps) +
                                        static_cast<uint32_t>(prod_c < kEps);
        if (number_outside == 3) {
            continue;
        }

        ClipTriangle(
            {.scalar_prod = prod_a, .index = id_a}, {.scalar_prod = prod_b, .index = id_b},
            {.scalar_prod = prod_c, .index = id_c}
        );
    }
}

void Clipper::ClipLine(ClippingPoint point_a, ClippingPoint point_b) {
    if (point_a.scalar_prod < -kEps) {
        std::swap(point_a, point_b);
    }
    assert(point_a.scalar_prod >= kEps && "Expected one point with positive scalar production with clip plane");
    assert(point_b.scalar_prod < -kEps && "Expected one point with negative scalar production with clip plane");

    AddInterpolatedPoint(point_a, point_b);
    line_indices_.emplace_back(point_a.index, vertices_.size() - 1);
}

void Clipper::ClipTriangle(ClippingPoint point_a, ClippingPoint point_b, ClippingPoint point_c) {
    SortValues<ClippingPoint>(point_a, point_b, point_c, [](auto left, auto right) {
        return left.scalar_prod < right.scalar_prod;
    });
    assert(
        point_a.scalar_prod >= kEps && "Expected at least one point with positive scalar production with clip plane"
    );

    if (point_b.scalar_prod < -kEps) {
        AddInterpolatedPoint(point_a, point_b);
        AddInterpolatedPoint(point_a, point_c);

        triangle_indices_.emplace_back(point_a.index, vertices_.size() - 2, vertices_.size() - 1);
    } else if (point_c.scalar_prod < -kEps) {
        AddInterpolatedPoint(point_a, point_c);
        AddInterpolatedPoint(point_b, point_c);

        triangle_indices_.emplace_back(point_a.index, point_b.index, vertices_.size() - 1);
        triangle_indices_.emplace_back(point_a.index, vertices_.size() - 2, vertices_.size() - 1);
    } else {
        assert(false && "Expected at least one point with negative scalar production with clip plane");
    }
}

void Clipper::AddInterpolatedPoint(ClippingPoint inside, ClippingPoint outside) {
    assert(inside.scalar_prod >= -kEps && "First point should be inside clip plane");
    assert(outside.scalar_prod < kEps && "Second point should be outside clip plane");

    auto vertex_a = vertices_[inside.index];
    auto vertex_b = vertices_[outside.index];

    const auto delta = inside.scalar_prod - outside.scalar_prod;
    if (delta < kEps) {
        vertex_a += vertex_b;
        vertex_a *= 0.5;
        vertices_.emplace_back(std::move(vertex_a));
        return;
    }

    const auto ratio = inside.scalar_prod / delta;
    vertex_a *= 1.0 - ratio;
    vertex_b *= ratio;

    vertex_a += vertex_b;
    vertices_.emplace_back(std::move(vertex_a));
}

void Clipper::ClipTrianglesByNormals(Vec3 view_pos) {
    if (triangle_indices_.empty()) {
        return;
    }

    for (int64_t i = triangle_indices_.size() - 1; i >= 0; --i) {
        const auto [id_a, id_b, id_c] = triangle_indices_[i];

        if (!IsWisiblePoint(vertices_[id_a], view_pos) && !IsWisiblePoint(vertices_[id_b], view_pos) &&
            !IsWisiblePoint(vertices_[id_c], view_pos)) {
            SwapRemove(triangle_indices_, i);
        }
    }
}

}  // namespace null_engine::detail
