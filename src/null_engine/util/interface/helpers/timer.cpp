#include "timer.hpp"

#include <SFML/System/Clock.hpp>

namespace null_engine {

namespace {

class TimerProvider : public ProviderBase<ITimerClient> {
    using Base = ProviderBase<ITimerClient>;

public:
    using Base::Base;

    void RefreshClients() {
        FloatType delta_time = static_cast<FloatType>(timer_.restart().asSeconds());
        for (const auto& [_, client] : clients_) {
            client->Update(delta_time);
        }
    }

private:
    sf::Clock timer_;
};

}  // anonymous namespace

folly::Poly<ITimerProvider> CreateTimerProvider() {
    return TimerProvider();
}

}  // namespace null_engine
