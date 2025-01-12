#include "tests_helpers.hpp"

namespace null_engine::tests {

drawable::VerticesObject CreatePointsSet(uint64_t number_points, util::Vec3 offset, util::Vec3 size, util::Vec3 color) {
    drawable::VerticesObject object(number_points * number_points);

    util::Vec3 step = size / static_cast<util::FloatType>(number_points);
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const util::Vec3 point =
                offset + step * util::Vec3(static_cast<util::FloatType>(i), static_cast<util::FloatType>(j));

            object.SetVertex(i * number_points + j, generic::Vertex(point, color));
        }
    }

    return object;
}

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

}  // namespace null_engine::tests
