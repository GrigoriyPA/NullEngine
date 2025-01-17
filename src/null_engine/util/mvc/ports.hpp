#pragma once

#include <functional>
#include <memory>
#include <unordered_set>

namespace null_engine {

template <typename Event>
class OutPort;

template <typename Event>
class InPort {
    friend class OutPort<Event>;

public:
    using EventsHandler = std::function<void(const Event& event)>;

    explicit InPort(EventsHandler handler)
        : events_handler_(handler) {
    }

    InPort(const InPort<Event>& other) = delete;
    InPort& operator=(const InPort<Event>& other) = delete;

    bool HasSubscription() const {
        return subscribed_port_ != nullptr;
    }

    void OnEvent(const Event& event) {
        events_handler_(event);
    }

    ~InPort() {
        DoUnsubscribe();
    }

private:
    void OnSubscribed(OutPort<Event>* out_port, const Event& event) {
        DoUnsubscribe();
        subscribed_port_ = out_port;
        OnEvent(event);
    }

    void OnUnsubscribed() {
        subscribed_port_ = nullptr;
    }

    void DoUnsubscribe() {
        if (HasSubscription()) {
            subscribed_port_->Unsubscribe(this);
        }
    }

    OutPort<Event>* subscribed_port_ = nullptr;
    EventsHandler events_handler_ = nullptr;
};

template <typename Event>
class OutPort {
public:
    using Ptr = std::unique_ptr<OutPort>;

    OutPort() = default;
    OutPort(const OutPort<Event>& other) = delete;
    OutPort& operator=(const OutPort<Event>& other) = delete;

    static OutPort::Ptr Make() {
        return std::make_unique<OutPort>();
    }

    void Subscribe(InPort<Event>* in_port, const Event& event) {
        if (subscriptions_.emplace(in_port).second) {
            in_port->OnSubscribed(this, event);
        }
    }

    void Unsubscribe(InPort<Event>* in_port) {
        const auto it = subscriptions_.find(in_port);
        if (it != subscriptions_.end()) {
            subscriptions_.erase(it);
            in_port->OnUnsubscribed();
        }
    }

    void Notify(const Event& event) {
        for (const auto in_port : subscriptions_) {
            in_port->OnEvent(event);
        }
    }

    ~OutPort() {
        for (const auto in_port : subscriptions_) {
            in_port->OnUnsubscribed();
        }
    }

private:
    std::unordered_set<InPort<Event>*> subscriptions_;
};

}  // namespace null_engine
