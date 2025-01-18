#pragma once

#include <null_engine/util/geometry/vector3.hpp>

namespace null_engine {

struct VertexParams {
    Vec3 color;
    Vec3 normal;
};

struct Vertex {
    Vec3 position;
    VertexParams params;
};

}  // namespace null_engine
