#include "native_vertex.hpp"

namespace null_engine::native {

//// Vertex

Vertex::Vertex(util::Vec3 position, util::Vec3 color)
    : position_(position)
    , color_(color) {
}

util::Vec3 Vertex::GetPosition() const {
    return position_;
}

util::Vec3 Vertex::GetColor() const {
    return color_;
}

}  // namespace null_engine::native
