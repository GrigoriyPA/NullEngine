#include "events.hpp"

namespace null_engine::util {

namespace {

class EventsProvider : public ProviderBase<IEventsClient> {
    using Base = ProviderBase<IEventsClient>;

public:
    using Base::Base;

    void DispatchEvent(const sf::Event& event) {
        for (const auto& [_, client] : clients_) {
            client->OnEvent(event);
        }
    }
};

}  // anonymous namespace

folly::Poly<IEventsProvider> CreateEventsProvider() {
    return EventsProvider();
}

}  // namespace null_engine::util
