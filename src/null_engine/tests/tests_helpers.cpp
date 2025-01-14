#include "tests_helpers.hpp"

namespace null_engine::tests {

VerticesObject CreatePointsSet(uint64_t number_points, Vec3 offset, Vec3 size, Vec3 color) {
    VerticesObject object(number_points * number_points);

    Vec3 step = size / static_cast<FloatType>(number_points);
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const Vec3 point = offset + step * Vec3(static_cast<FloatType>(i), static_cast<FloatType>(j));

            object.SetVertex(i * number_points + j, {.position = point, .params = {.color = color}});
        }
    }

    return object;
}

void DrawPoints(
    Rasterizer& rasterizer, RasterizerBuffer& buffer, uint64_t number_points, Vec3 offset, Vec3 size, Vec3 color
) {
    Vec3 step = size / static_cast<FloatType>(number_points);
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const Vec3 point = offset + step * Vec3(static_cast<FloatType>(i), static_cast<FloatType>(j));

            rasterizer.DrawPoint({.position = point, .params = {.color = color}}, buffer);
        }
    }
}

}  // namespace null_engine::tests
