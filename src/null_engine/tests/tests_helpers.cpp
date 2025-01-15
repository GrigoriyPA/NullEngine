#include "tests_helpers.hpp"

namespace null_engine::tests {

VerticesObject CreatePointsSet(uint64_t number_points, Vec3 offset, Vec2 size, Vec3 color) {
    VerticesObject object(number_points * number_points);

    const Vec2 step = size / number_points;
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const Vec3 point = offset + Vec3(step * Vec2(i, j), 0.0);

            object.SetVertex(i * number_points + j, {.position = point, .params = {.color = color}});
        }
    }

    return object;
}

void DrawPoints(
    const detail::Rasterizer& rasterizer, detail::RasterizerBuffer& buffer, uint64_t number_points, Vec3 offset,
    Vec2 size, Vec3 color
) {
    const Vec2 step = size / number_points;
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const Vec3 point = offset + Vec3(step * Vec2(i, j), 0.0);

            rasterizer.DrawPoint({.position = point, .params = {.color = color}}, buffer);
        }
    }
}

}  // namespace null_engine::tests
