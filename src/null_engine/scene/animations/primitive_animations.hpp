#pragma once

#include "animator.hpp"

namespace null_engine {

class RotationAnimation : public TimedAnimator {
public:
    RotationAnimation(Vec3 axis, FloatType rotation_speed);

private:
    void OnRefresh(FloatType delta_time);

    Vec3 axis_;
    FloatType rotation_speed_;
};

class TranslationAnimation : public TimedAnimator {
public:
    TranslationAnimation(Vec3 start, Vec3 end, FloatType speed);

private:
    void OnRefresh(FloatType delta_time);

    Vec3 start_;
    Vec3 end_;
    FloatType period_;
    FloatType time_ = 0.0;
};

}  // namespace null_engine
