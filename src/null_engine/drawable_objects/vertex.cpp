#include "vertex.hpp"

namespace null_engine {

Vertex::Vertex(Vec3 position, Vec3 color)
    : position_(position)
    , color_(color) {
}

Vec3 Vertex::GetPosition() const {
    return position_;
}

Vec3 Vertex::GetColor() const {
    return color_;
}

Vertex& Vertex::SetPosition(Vec3 position) {
    position_ = position;
    return *this;
}

Vertex& Vertex::SetColor(Vec3 color) {
    color_ = color;
    return *this;
}

}  // namespace null_engine
