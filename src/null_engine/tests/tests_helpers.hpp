#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/native/rasterization/native_rasterizer.hpp>

namespace null_engine::tests {

drawable::VerticesObject CreatePointsSet(uint64_t number_points, util::Vec3 offse, util::Vec3 size, util::Vec3 color);

void DrawPoints(
    native::Rasterizer& rasterizer, uint64_t number_points, util::Vec3 offset, util::Vec3 size, util::Vec3 color
);

}  // namespace null_engine::tests
