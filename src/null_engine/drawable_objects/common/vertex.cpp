#include "vertex.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

VertexParams& VertexParams::operator+=(const VertexParams& other) {
    color += other.color;
    return *this;
}

VertexParams& VertexParams::operator-=(const VertexParams& other) {
    color -= other.color;
    return *this;
}

VertexParams& VertexParams::operator*=(FloatType scale) {
    color *= scale;
    return *this;
}

VertexParams& VertexParams::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    color /= scale;
    return *this;
}

Vertex& Vertex::operator+=(const Vertex& other) {
    position += other.position;
    params += other.params;
    return *this;
}

Vertex& Vertex::operator-=(const Vertex& other) {
    position -= other.position;
    params -= other.params;
    return *this;
}

Vertex& Vertex::operator*=(FloatType scale) {
    position *= scale;
    params *= scale;
    return *this;
}

Vertex& Vertex::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    position /= scale;
    params /= scale;
    return *this;
}

}  // namespace null_engine
