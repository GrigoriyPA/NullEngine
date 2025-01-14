#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/native/rasterization/native_rasterizer.hpp>

namespace null_engine::tests {

VerticesObject CreatePointsSet(uint64_t number_points, Vec3 offse, Vec3 size, Vec3 color);

void DrawPoints(native::Rasterizer& rasterizer, uint64_t number_points, Vec3 offset, Vec3 size, Vec3 color);

}  // namespace null_engine::tests
