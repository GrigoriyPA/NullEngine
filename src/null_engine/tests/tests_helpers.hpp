#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/renderer/rasterization/native_rasterizer.hpp>
#include <null_engine/renderer/shaders/fragment_shader_interface.hpp>

namespace null_engine::tests {

VerticesObject CreatePointsSet(uint64_t number_points, Vec3 offse, Vec2 size, Vec3 color);

void DrawPoints(
    const native::detail::Rasterizer& rasterizer, native::detail::RasterizerBuffer& buffer,
    native::detail::AnyFragmentShaderRef fragment_shader, uint64_t number_points, Vec3 offset, Vec2 size, Vec3 color
);

}  // namespace null_engine::tests
