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
    using Ptr = std::unique_ptr<InPort>;
    using EventsHandler = std::function<void(const Event& event)>;

    explicit InPort(EventsHandler handler)
        : events_handler_(handler) {
    }

    static InPort::Ptr Make(EventsHandler handler) {
        return std::make_unique<InPort>(std::move(handler));
    }

    ~InPort() {
        DoUnsubscribe();
    }

private:
    void OnEvent(const Event& event) {
        events_handler_(event);
    }

    void OnSubscribed(OutPort<Event>* out_port) {
        DoUnsubscribe();
        subscribed_port_ = out_port;
    }

    void OnUnsubscribed() {
        subscribed_port_ = nullptr;
    }

    void DoUnsubscribe() {
        if (subscribed_port_) {
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

    static OutPort::Ptr Make() {
        return std::make_unique<OutPort>();
    }

    void Subscribe(InPort<Event>* in_port) {
        if (subscriptions_.emplace(in_port).second) {
            in_port->OnSubscribed(this);
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
