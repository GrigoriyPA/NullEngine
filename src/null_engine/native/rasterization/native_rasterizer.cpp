#include <null_engine/native/rasterization/native_rasterizer.hpp>

#include <cstdint>
#include <utility>

namespace null_engine::native {

//// Rasterizer

Rasterizer::Rasterizer(RasterizerContext& context)
    : context_(context)
    , pixel_height_(2.0 / static_cast<util::FloatType>(context_.view_height))
    , pixel_width_(2.0 / static_cast<util::FloatType>(context_.view_width)) {
}

void Rasterizer::DrawPoint(generic::Vertex point) {
    const auto [x, y] = ProjectPoint(point);

    if (x < 0 || context_.view_width <= x || y < 0 || context_.view_height < y) {
        return;
    }

    if (!CheckPointDepth(x, y, point)) {
        return;
    }

    UpdateViewPixel(x, y, point);
}

std::pair<int64_t, int64_t> Rasterizer::ProjectPoint(const generic::Vertex& point) const {
    const auto position = point.GetPosition();
    return std::make_pair(
        static_cast<int64_t>((position.GetX() + 1.0) / pixel_width_),
        static_cast<int64_t>((1.0 - position.GetY()) / pixel_width_)
    );
}

bool Rasterizer::CheckPointDepth(int64_t x, int64_t y, const generic::Vertex& point) const {
    const auto z = point.GetPosition().GetZ();
    if (z <= -1.0 || 1.0 <= z) {
        return false;
    }
    return context_.depth_buffer[y * context_.view_width + x] < z;
}

void Rasterizer::UpdateViewPixel(int64_t x, int64_t y, const generic::Vertex& point) {
    const auto point_offset = y * context_.view_width + x;
    context_.depth_buffer[point_offset] = point.GetPosition().GetZ();

    const auto color = (point.GetColor() * 255.0).Clamp(0.0, 255.0);
    context_.colors_buffer[4 * point_offset] = static_cast<uint8_t>(color.GetX());
    context_.colors_buffer[4 * point_offset + 1] = static_cast<uint8_t>(color.GetY());
    context_.colors_buffer[4 * point_offset + 2] = static_cast<uint8_t>(color.GetZ());
    context_.colors_buffer[4 * point_offset + 3] = 255;
}

namespace tests {

//// Test functions

void DrawPoints(
    native::Rasterizer& rasterizer, uint64_t number_points, util::Vec3 offset, util::Vec3 size, util::Vec3 color
) {
    util::Vec3 step = size / static_cast<util::FloatType>(number_points);
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const util::Vec3 point =
                offset + step * util::Vec3(static_cast<util::FloatType>(i), static_cast<util::FloatType>(j));

            rasterizer.DrawPoint(generic::Vertex(point, color));
        }
    }
}

}  // namespace tests

}  // namespace null_engine::native
