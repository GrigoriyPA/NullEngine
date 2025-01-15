#pragma once

#include <null_engine/util/geometry/vector3.hpp>

namespace null_engine {

class VertexParams {
public:
    Vec3 color;

    VertexParams& operator+=(const VertexParams& other);

    VertexParams& operator-=(const VertexParams& other);

    VertexParams& operator*=(FloatType scale);

    VertexParams& operator/=(FloatType scale);
};

struct Vertex {
    Vec3 position;
    VertexParams params;
};

}  // namespace null_engine
