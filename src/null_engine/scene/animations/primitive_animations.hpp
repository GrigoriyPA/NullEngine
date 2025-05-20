#pragma once

#include "animator.hpp"

namespace null_engine {

class RotationAnimation : public TimedAnimator {
public:
    RotationAnimation(Vec3 axis, float rotation_speed);

private:
    void OnRefresh(float delta_time);

    Vec3 axis_;
    float rotation_speed_;
};

class TranslationAnimation : public TimedAnimator {
public:
    TranslationAnimation(Vec3 start, Vec3 end, float speed);

private:
    void OnRefresh(float delta_time);

    Vec3 start_;
    Vec3 end_;
    float period_;
    float time_ = 0.0;
};

}  // namespace null_engine
