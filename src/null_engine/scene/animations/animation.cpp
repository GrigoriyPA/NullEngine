#include "animation.hpp"

#include <cassert>

namespace null_engine {

Animation::Animation()
    : in_transform_port_(std::bind(&Animation::OnUpdateTransform, this, std::placeholders::_1)) {
}

Animation::Ptr Animation::Make() {
    return std::make_unique<Animation>();
}

InPort<Mat4>* Animation::GetTransformPort() {
    return &in_transform_port_;
}

const Mat4& Animation::GetTransform() const {
    return transform_;
}

bool Animation::Initialized() const {
    return in_transform_port_.HasSubscription();
}

void Animation::OnUpdateTransform(const Mat4& transform) {
    transform_ = transform;
}

}  // namespace null_engine
