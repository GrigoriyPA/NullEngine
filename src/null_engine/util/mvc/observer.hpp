#pragma once

#include <optional>

#include "ports.hpp"

namespace null_engine {

template <typename Event>
class Observer {
public:
    using Ptr = std::unique_ptr<Observer>;

    Observer()
        : in_event_port_(std::bind(&Observer::OnUpdateState, this, std::placeholders::_1)) {
    }

    static Observer::Ptr Make() {
        return std::make_unique<Observer>();
    }

    InPort<Event>* GetInPort() {
        return &in_event_port_;
    }

    const std::optional<Event>& GetState() const {
        return state_;
    }

    bool Initialized() const {
        return in_event_port_.HasSubscription();
    }

private:
    void OnUpdateState(const Event& event) {
        state_ = event;
    }

    std::optional<Event> state_;
    InPort<Event> in_event_port_;
};

}  // namespace null_engine
