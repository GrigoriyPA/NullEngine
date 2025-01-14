#include "rasterizer.hpp"

namespace null_engine {

namespace {

VertexParams Interpolate(
    const VertexParams& point_a, const VertexParams& point_b, const VertexParams& point_c, Vec3 perspective
) {
    const auto color =
        point_a.color * perspective.X() + point_b.color * perspective.Y() + point_b.color * perspective.Z();
    return {.color = color};
}

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height)
    : view_width_(view_width)
    , view_height_(view_height)
    , pixel_height_(2.0 / static_cast<FloatType>(view_height))
    , pixel_width_(2.0 / static_cast<FloatType>(view_width)) {
}

void Rasterizer::DrawPoint(const Vertex& point, RasterizerBuffer& buffer) const {
    const Vec3& position = point.position;

    const int64_t x = (position.GetX() + 1.0) / pixel_width_;
    const int64_t y = (1.0 - position.GetY()) / pixel_height_;

    if (x < 0 || view_width_ <= x || y < 0 || view_height_ <= y) {
        return;
    }

    const auto z = position.GetZ();
    if (!CheckPointDepth(x, y, z, buffer)) {
        return;
    }

    UpdateViewPixel(x, y, z, point.params, buffer);
}

void Rasterizer::DrawTriangle(
    const Vertex& point_a, const Vertex& point_b, const Vertex& point_c, RasterizerBuffer& buffer
) const {
    const Vec3 positions_z(point_a.position.GetZ(), point_b.position.GetZ(), point_c.position.GetZ());
    const Vec3 positions_h(point_a.position.GetH(), point_b.position.GetH(), point_c.position.GetH());

    const auto denom = 1.0 / Area(point_a.position.GetXY(), point_b.position.GetXY(), point_c.position.GetXY());
    for (int64_t x = 0; x < view_width_; ++x) {
        for (int64_t y = 0; y < view_height_; ++y) {
            const Vec2 view_position = Vec2(x * pixel_width_ - 1.0, 1.0 - y * pixel_height_);

            const Vec3 barycentric =
                denom * Vec3(
                            Area(view_position, point_a.position.GetXY(), point_b.position.GetXY()),
                            Area(view_position, point_a.position.GetXY(), point_c.position.GetXY()),
                            Area(view_position, point_b.position.GetXY(), point_c.position.GetXY())
                        );

            if (barycentric.GetX() < 0.0 || barycentric.GetY() < 0.0 || barycentric.GetZ() < 0.0) {
                continue;
            }

            const Vec3 frag_position(
                view_position.GetX(), view_position.GetY(), positions_z.ScalarProd(barycentric),
                positions_h.ScalarProd(barycentric)
            );

            const auto z = frag_position.GetZ();
            if (CheckPointDepth(x, y, z, buffer)) {
                continue;
            }

            const Vec3 perspective = barycentric * positions_h / frag_position.GetH();
            const VertexParams params = Interpolate(point_a.params, point_b.params, point_c.params, perspective);
            UpdateViewPixel(x, y, z, params, buffer);
        }
    }
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
    buffer.colors[4 * point_offset] = static_cast<uint8_t>(color.GetX());
    buffer.colors[4 * point_offset + 1] = static_cast<uint8_t>(color.GetY());
    buffer.colors[4 * point_offset + 2] = static_cast<uint8_t>(color.GetZ());
    buffer.colors[4 * point_offset + 3] = 255;
}

}  // namespace null_engine
