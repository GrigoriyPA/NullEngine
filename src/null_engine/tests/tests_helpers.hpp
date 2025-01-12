#pragma once

#include <null_engine/drawable_objects/common/vertices_object.hpp>
#include <null_engine/native/rasterization/native_rasterizer.hpp>

namespace null_engine::tests {

drawable::VerticesObject CreatePointsSet(
    uint64_t number_points, util::Vec3 offset = util::Vec3(-0.5, -0.5), util::Vec3 size = util::Vec3(1.0, 1.0),
    util::Vec3 color = util::Vec3(1.0)
);

void DrawPoints(
    native::Rasterizer& rasterizer, uint64_t number_points, util::Vec3 offset = util::Vec3(-0.5, -0.5),
    util::Vec3 size = util::Vec3(1.0, 1.0), util::Vec3 color = util::Vec3(1.0)
);

}  // namespace null_engine::tests
