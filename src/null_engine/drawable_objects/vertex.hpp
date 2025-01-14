#pragma once

#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine {

struct VertexParams {
    Vec3 color;
};

struct Vertex {
    Vec3 position;
    VertexParams params;
};

}  // namespace null_engine
