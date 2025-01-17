#pragma once

#include <null_engine/util/geometry/matrix4.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

class Animator {
public:
    void SubscribeOnAnimation(InPort<Mat4>* observer_port) const;

    const Mat4& GetCurrentTransform() const;

    void UpdateTransform(const Mat4& transform);

private:
    Mat4 current_transform_;
    OutPort<Mat4>::Ptr out_transform_port_ = OutPort<Mat4>::Make();
};

class TimedAnimator : public Animator {
    using InTimePort = InPort<FloatType>;

public:
    explicit TimedAnimator(InTimePort::EventsHandler refresh_handler);

    InTimePort* GetRefreshPort();

private:
    InTimePort in_time_port_;
};

class AnimatorRegistry {
public:
    AnimatorRegistry();

    InPort<FloatType>* GetRefreshPort();

    void AddAnimator(std::unique_ptr<TimedAnimator> animator);

private:
    void OnRefresh(FloatType delta_time) const;

    std::vector<std::unique_ptr<TimedAnimator>> animators_;
    InPort<FloatType> in_time_port_;
    OutPort<FloatType>::Ptr out_time_port_ = OutPort<FloatType>::Make();
};

}  // namespace null_engine
