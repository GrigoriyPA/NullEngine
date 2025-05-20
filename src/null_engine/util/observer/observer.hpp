#pragma once

#include <optional>

#include "ports.hpp"

namespace null_engine {

template <typename Event>
class Observer {
public:
    using Uptr = std::unique_ptr<Observer>;

    Observer()
        : in_event_port_(std::bind(&Observer::OnUpdateState, this, std::placeholders::_1)) {
    }

    static Observer::Uptr Make() {
        return std::make_unique<Observer>();
    }

    InPort<Event>* GetInPort() {
        return &in_event_port_;
    }

    const std::optional<Event>& GetLastData() const {
        return last_data_;
    }

    bool IsSubscribed() const {
        return in_event_port_.HasSubscription();
    }

private:
    void OnUpdateState(const Event& event) {
        last_data_ = event;
    }

    std::optional<Event> last_data_;
    InPort<Event> in_event_port_;
};

}  // namespace null_engine
