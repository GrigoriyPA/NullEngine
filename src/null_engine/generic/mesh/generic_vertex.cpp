#include <null_engine/generic/mesh/generic_vertex.hpp>

namespace null_engine::generic {

//// Vertex

Vertex::Vertex(util::Vec3 position, util::Vec3 color)
    : position_(position)
    , color_(color) {
}

Vertex& Vertex::SetPosition(util::Vec3 position) {
    position_ = position;
    return *this;
}

Vertex& Vertex::SetColor(util::Vec3 color) {
    color_ = color;
    return *this;
}

util::Vec3 Vertex::GetPosition() const {
    return position_;
}

util::Vec3 Vertex::GetColor() const {
    return color_;
}

}  // namespace null_engine::generic
