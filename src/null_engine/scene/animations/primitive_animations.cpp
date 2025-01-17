#include "primitive_animations.hpp"

namespace null_engine {

RotationAnimation::RotationAnimation(Vec3 axis, FloatType rotation_speed)
    : TimedAnimator(std::bind(&RotationAnimation::OnRefresh, this, std::placeholders::_1))
    , axis_(axis.Normalize())
    , rotation_speed_(rotation_speed) {
}

void RotationAnimation::OnRefresh(FloatType delta_time) {
    const auto change = Mat4::Rotation(axis_, rotation_speed_ * delta_time);

    UpdateTransform(change * GetCurrentTransform());
}

}  // namespace null_engine
