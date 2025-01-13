#include "generic_vertex.hpp"

namespace null_engine::generic {

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

Vertex& Vertex::SetPosition(util::Vec3 position) {
    position_ = position;
    return *this;
}

Vertex& Vertex::SetColor(util::Vec3 color) {
    color_ = color;
    return *this;
}

void Vertex::ApplyNdcTransform(const util::Transform& ndc_transform) {
    position_ = ndc_transform.Apply(position_);
    position_ /= position_.GetH();
}

}  // namespace null_engine::generic
