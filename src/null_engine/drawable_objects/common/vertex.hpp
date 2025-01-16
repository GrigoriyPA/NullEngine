#pragma once

#include <null_engine/util/geometry/vector4.hpp>

namespace null_engine {

class VertexParams {
public:
    Vec3 color;

    VertexParams& operator+=(const VertexParams& other);

    VertexParams& operator-=(const VertexParams& other);

    VertexParams& operator*=(FloatType scale);

    VertexParams& operator/=(FloatType scale);
};

class Vertex {
public:
    Vec4 position;
    VertexParams params;

    Vertex& operator+=(const Vertex& other);

    Vertex& operator-=(const Vertex& other);

    Vertex& operator*=(FloatType scale);

    Vertex& operator/=(FloatType scale);
};

}  // namespace null_engine
