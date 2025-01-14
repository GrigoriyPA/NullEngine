#include "controller.hpp"

#include <SFML/Window/Event.hpp>

namespace null_engine::tests {

Controller::Controller(sf::RenderWindow& window)
    : window_(window)
    , mouse_control_(window)
    , in_refresh_port_(InPort<FloatType>::Make(std::bind(&Controller::OnRefresh, this, std::placeholders::_1))) {
    out_events_port_->Subscribe(mouse_control_.GetEventsPort());
}

InPort<FloatType>* Controller::GetRefreshPort() const {
    return in_refresh_port_.get();
}

void Controller::SubscribeToMouseMove(InPort<Vec2>* observer_port) const {
    mouse_control_.SubscribeOnMouseMove(observer_port);
}

void Controller::OnRefresh(FloatType) const {
    for (sf::Event event; window_.pollEvent(event);) {
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
            window_.close();
            return;
        }

        out_events_port_->Notify(event);
    }
}

}  // namespace null_engine::tests
