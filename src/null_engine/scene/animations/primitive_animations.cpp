#include "primitive_animations.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

RotationAnimation::RotationAnimation(Vec3 axis, FloatType rotation_speed)
    : TimedAnimator(std::bind(&RotationAnimation::OnRefresh, this, std::placeholders::_1))
    , axis_(axis.normalized())
    , rotation_speed_(rotation_speed) {
}

void RotationAnimation::OnRefresh(FloatType delta_time) {
    const auto change = Rotation(axis_, rotation_speed_ * delta_time);

    UpdateTransform(change * GetCurrentTransform());
}

TranslationAnimation::TranslationAnimation(Vec3 start, Vec3 end, FloatType speed)
    : TimedAnimator(std::bind(&TranslationAnimation::OnRefresh, this, std::placeholders::_1))
    , start_(start)
    , end_(end)
    , period_(2.0 * (start - end).norm() / speed) {
    assert(period_ > kEps && "Invalid translation animation points");
}

void TranslationAnimation::OnRefresh(FloatType delta_time) {
    time_ = Module(time_ + delta_time, period_);

    FloatType ratio = 2.0 * time_ / period_;
    if (ratio > 1.0) {
        ratio = 2.0 - ratio;
    }
    UpdateTransform(Translation(ratio * start_ + (1.0 - ratio) * end_));
}

}  // namespace null_engine
