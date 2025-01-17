#include "animator.hpp"

namespace null_engine {

void Animator::SubscribeOnAnimation(InPort<Mat4>* observer_port) const {
    out_transform_port_->Subscribe(observer_port, current_transform_);
}

const Mat4& Animator::GetCurrentTransform() const {
    return current_transform_;
}

void Animator::UpdateTransform(const Mat4& transform) {
    current_transform_ = transform;
    out_transform_port_->Notify(transform);
}

TimedAnimator::TimedAnimator(InTimePort::EventsHandler refresh_handler)
    : in_time_port_(std::move(refresh_handler)) {
}

TimedAnimator::InTimePort* TimedAnimator::GetRefreshPort() {
    return &in_time_port_;
}

AnimatorRegistry::AnimatorRegistry()
    : in_time_port_(std::bind(&AnimatorRegistry::OnRefresh, this, std::placeholders::_1)) {
}

InPort<FloatType>* AnimatorRegistry::GetRefreshPort() {
    return &in_time_port_;
}

void AnimatorRegistry::AddAnimator(std::unique_ptr<TimedAnimator> animator) {
    out_time_port_->Subscribe(animator->GetRefreshPort(), 0.0);
    animators_.emplace_back(std::move(animator));
}

void AnimatorRegistry::OnRefresh(FloatType delta_time) const {
    out_time_port_->Notify(delta_time);
}

}  // namespace null_engine
