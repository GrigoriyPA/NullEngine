#pragma once

#include <null_engine/util/geometry/vector.hpp>

namespace null_engine {

struct VertexParams {
    Vec3 color = Vec3(0.0, 0.0, 0.0);
    Vec3 normal = Vec3(0.0, 0.0, 0.0);
    Vec2 tex_coords = Vec2(0.0, 0.0);
};

struct Vertex {
    Vec3 position = Vec3(0.0, 0.0, 0.0);
    VertexParams params;
};

}  // namespace null_engine
