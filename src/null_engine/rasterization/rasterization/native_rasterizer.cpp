#include "native_rasterizer.hpp"

#include <cstdint>
#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::native {

namespace {

Vertex Interpolate(Vertex point_a, Vertex point_b, Vertex point_c, Vec3 frag_position, Vec3 perspective) {
    const auto color = point_a.GetColor() * perspective.GetX() + point_b.GetColor() * perspective.GetY() +
                       point_b.GetColor() * perspective.GetZ();
    return Vertex(frag_position, color);
}

}  // anonymous namespace

Rasterizer::Rasterizer(RasterizerContext context)
    : context_(context)
    , pixel_height_(2.0 / static_cast<FloatType>(context_.view_height))
    , pixel_width_(2.0 / static_cast<FloatType>(context_.view_width)) {
}

const RasterizerContext& Rasterizer::GetContext() const {
    return context_;
}

RasterizerContext& Rasterizer::GetContext() {
    return context_;
}

void Rasterizer::DrawPoint(Vertex point) {
    const auto [x, y] = ProjectPoint(point);

    if (x < 0 || context_.view_width <= x || y < 0 || context_.view_height <= y) {
        return;
    }

    if (!CheckPointDepth(x, y, point.GetPosition().GetZ())) {
        return;
    }

    UpdateViewPixel(x, y, point);
}

void Rasterizer::DrawTriangle(Vertex point_a, Vertex point_b, Vertex point_c) {
    const Vec3 positions[3] = {point_a.GetPosition(), point_b.GetPosition(), point_b.GetPosition()};
    const Vec3 positions_z(positions[0].GetZ(), positions[1].GetZ(), positions[2].GetZ());
    const Vec3 positions_h(positions[0].GetH(), positions[1].GetH(), positions[2].GetH());

    const FloatType denom = 1.0 / Area(positions[0].GetXY(), positions[1].GetXY(), positions[2].GetXY());
    for (int64_t x = 0; x < context_.view_width; ++x) {
        for (int64_t y = 0; y < context_.view_height; ++y) {
            const Vec2 view_position =
                Vec2(static_cast<FloatType>(x) * pixel_width_ - 1.0, static_cast<FloatType>(y) * pixel_height_);

            const Vec3 barycentric = denom * Vec3(
                                                 Area(view_position, positions[0].GetXY(), positions[1].GetXY()),
                                                 Area(view_position, positions[0].GetXY(), positions[2].GetXY()),
                                                 Area(view_position, positions[1].GetXY(), positions[2].GetXY())
                                             );

            if (barycentric.GetX() < 0.0 || barycentric.GetY() < 0.0 || barycentric.GetZ() < 0.0) {
                continue;
            }

            const Vec3 frag_position(
                view_position.GetX(), view_position.GetY(), positions_z.ScalarProd(barycentric),
                positions_h.ScalarProd(barycentric)
            );

            if (CheckPointDepth(x, y, frag_position.GetZ())) {
                continue;
            }

            const Vec3 perspective = barycentric * positions_h / frag_position.GetH();

            UpdateViewPixel(x, y, Interpolate(point_a, point_b, point_c, frag_position, perspective));
        }
    }
}

std::pair<int64_t, int64_t> Rasterizer::ProjectPoint(const Vertex& point) const {
    const auto position = point.GetPosition();
    return std::make_pair(
        static_cast<int64_t>((position.GetX() + 1.0) / pixel_width_),
        static_cast<int64_t>((1.0 - position.GetY()) / pixel_height_)
    );
}

bool Rasterizer::CheckPointDepth(int64_t x, int64_t y, FloatType z) const {
    if (z <= -1.0 || 1.0 <= z) {
        return false;
    }
    return context_.depth_buffer[y * context_.view_width + x] > z;
}

void Rasterizer::UpdateViewPixel(int64_t x, int64_t y, const Vertex& point) {
    const auto point_offset = y * context_.view_width + x;
    context_.depth_buffer[point_offset] = point.GetPosition().GetZ();

    const auto color = (point.GetColor() * 255.0).Clamp(0.0, 255.0);
    context_.colors_buffer[4 * point_offset] = static_cast<uint8_t>(color.GetX());
    context_.colors_buffer[4 * point_offset + 1] = static_cast<uint8_t>(color.GetY());
    context_.colors_buffer[4 * point_offset + 2] = static_cast<uint8_t>(color.GetZ());
    context_.colors_buffer[4 * point_offset + 3] = 255;
}

}  // namespace null_engine::native
