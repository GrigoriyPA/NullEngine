#include "rasterizer.hpp"

#include <cassert>
#include <null_engine/util/geometry/line2.hpp>

namespace null_engine::detail {

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height)
    : view_width_(view_width)
    , view_height_(view_height)
    , pixel_height_(2.0 / static_cast<FloatType>(view_height))
    , pixel_width_(2.0 / static_cast<FloatType>(view_width)) {
}

void Rasterizer::DrawPoint(const Vertex& point, RasterizerBuffer& buffer) const {
    const Vec3& position = point.position;

    const int64_t x = (position.X() + 1.0) / pixel_width_;
    const int64_t y = (1.0 - position.Y()) / pixel_height_;
    if (!CheckPointPosition(x, y)) {
        return;
    }

    const auto z = position.Z();
    if (!CheckPointDepth(x, y, z, buffer)) {
        return;
    }

    UpdateViewPixel(x, y, z, point.params, buffer);
}

void Rasterizer::DrawTriangle(Vertex point_a, Vertex point_b, Vertex point_c, RasterizerBuffer& buffer) const {
    if (point_a.position.Y() < point_b.position.Y()) {
        std::swap(point_a, point_b);
    }
    if (point_b.position.Y() < point_c.position.Y()) {
        std::swap(point_b, point_c);
        if (point_a.position.Y() < point_b.position.Y()) {
            std::swap(point_a, point_b);
        }
    }

    const Vec2 pos_a = point_a.position.XY();
    const Vec2 pos_b = point_b.position.XY();
    const Vec2 pos_c = point_c.position.XY();

    const Interpolation interp_a(point_a);
    const Interpolation interp_b(point_b);
    const Interpolation interp_c(point_c);

    const int64_t up_pixel = std::ceil((1.0 - pos_a.Y()) / pixel_height_);
    const int64_t middle_pixel = std::floor((1.0 - pos_b.Y()) / pixel_height_);
    const int64_t down_pixel = std::floor((1.0 - pos_c.Y()) / pixel_height_) - 1;

    if (down_pixel < up_pixel) {
        return;
    }

    const uint64_t height = down_pixel - up_pixel + 1;
    const auto middle_pos = Line2(pos_a, pos_c).IntersectHorizontal(pos_b.Y()).X();
    const bool swap_borders = OrientedArea(pos_a, pos_b, pos_c) < 0.0;

    if (up_pixel <= middle_pixel) {
        const uint64_t number_pixels = middle_pixel - up_pixel + 1;
        TriangleBorders::Border left = {
            .border = DirValue<FloatType>(pos_a.X(), middle_pos, number_pixels),
            .interpolation = DirValue<Interpolation>(interp_a, interp_c, height)
        };

        TriangleBorders::Border right = {
            .border = DirValue<FloatType>(pos_a.X(), pos_b.X(), number_pixels),
            .interpolation = DirValue<Interpolation>(interp_a, interp_b, number_pixels)
        };

        if (swap_borders) {
            std::swap(left, right);
        }

        RasterizeTriangleHalf(TriangleBorders(DirValue<int64_t>(up_pixel, 1), left, right, number_pixels), buffer);
    }

    if (middle_pixel < down_pixel) {
        const uint64_t number_pixels = down_pixel - middle_pixel;
        TriangleBorders::Border left = {
            .border = DirValue<FloatType>(pos_c.X(), middle_pos, number_pixels),
            .interpolation = DirValue<Interpolation>(interp_c, interp_a, height)
        };

        TriangleBorders::Border right = {
            .border = DirValue<FloatType>(pos_c.X(), pos_b.X(), number_pixels),
            .interpolation = DirValue<Interpolation>(interp_c, interp_b, number_pixels)
        };

        if (swap_borders) {
            std::swap(left, right);
        }

        RasterizeTriangleHalf(TriangleBorders(DirValue<int64_t>(down_pixel, -1), left, right, number_pixels), buffer);
    }
}

void Rasterizer::RasterizeTriangleHalf(TriangleBorders borders, RasterizerBuffer& buffer) const {
    for (; borders.HasPixels(); borders.Increment()) {
        const int64_t left_pixel = std::ceil((borders.GetLeftBorder() + 1.0) / pixel_width_);
        const int64_t right_pixel = std::floor((borders.GetRightBorder() + 1.0) / pixel_width_) - 1;
        if (right_pixel < left_pixel) {
            continue;
        }

        const uint64_t number_pixels = right_pixel - left_pixel + 1;
        RasterizeLine(
            RsteriztionLine(
                DirValue<int64_t>(left_pixel, 1), borders.GetY(),
                DirValue<Interpolation>(borders.GetLeftInterpolation(), borders.GetRightInterpolation(), number_pixels),
                number_pixels
            ),
            buffer
        );
    }
}

void Rasterizer::RasterizeLine(RsteriztionLine line, RasterizerBuffer& buffer) const {
    for (; line.HasPixels(); line.Increment()) {
        const auto x = line.GetX();
        const auto y = line.GetY();

        if (!CheckPointPosition(x, y)) {
            continue;
        }

        const auto& interpolation = line.GetInterpolation();
        const auto z = interpolation.GetZ();
        if (!CheckPointDepth(x, y, z, buffer)) {
            continue;
        }

        auto params = interpolation.GetParams();
        params /= interpolation.GetH();
        UpdateViewPixel(x, y, z, params, buffer);
    }
}

bool Rasterizer::CheckPointPosition(int64_t x, int64_t y) const {
    return 0 <= x && x < view_width_ && 0 <= y && y < view_height_;
}

bool Rasterizer::CheckPointDepth(int64_t x, int64_t y, FloatType z, RasterizerBuffer& buffer) const {
    if (z <= -1.0 || 1.0 <= z) {
        return false;
    }
    return buffer.depth[y * view_width_ + x] > z;
}

void Rasterizer::UpdateViewPixel(
    int64_t x, int64_t y, FloatType z, const VertexParams& point_params, RasterizerBuffer& buffer
) const {
    const uint64_t point_offset = y * view_width_ + x;
    buffer.depth[point_offset] = z;

    const Vec3 color = (point_params.color * 255.0).Clamp(0.0, 255.0);
    buffer.colors[4 * point_offset] = static_cast<uint8_t>(color.X());
    buffer.colors[4 * point_offset + 1] = static_cast<uint8_t>(color.Y());
    buffer.colors[4 * point_offset + 2] = static_cast<uint8_t>(color.Z());
    buffer.colors[4 * point_offset + 3] = 255;
}

}  // namespace null_engine::detail
