#pragma once

#include <null_engine/util/geometry/matrix.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

class Animator {
public:
    void SubscribeOnAnimation(InPort<Transform>* observer_port) const;

    const Transform& GetCurrentTransform() const;

    void UpdateTransform(const Transform& transform);

private:
    Transform current_transform_ = Ident();
    OutPort<Transform>::Ptr out_transform_port_ = OutPort<Transform>::Make();
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
