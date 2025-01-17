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

}  // namespace null_engine
