#include "centering_mouse_control.hpp"

#include <SFML/Window/Event.hpp>

namespace null_engine {

CenteringMouseControl::CenteringMouseControl(sf::RenderWindow& window)
    : window_(window)
    , window_width_(window_.getSize().x)
    , window_height_(window_.getSize().y)
    , in_events_port_(InPort<sf::Event>::Make(std::bind(&CenteringMouseControl::OnEvent, this, std::placeholders::_1))
      ) {
    window_.setMouseCursorVisible(false);
    CenteringMouse();
}

InPort<sf::Event>* CenteringMouseControl::GetEventsPort() const {
    return in_events_port_.get();
}

void CenteringMouseControl::SubscribeOnMouseMove(InPort<Vec2>* observer_port) const {
    out_mouse_move_port_->Subscribe(observer_port);
}

void CenteringMouseControl::OnEvent(const sf::Event& event) const {
    if (event.type != sf::Event::MouseMoved) {
        return;
    }

    Vec2 mouse_move(event.mouseMove.x - window_width_ / 2, event.mouseMove.y - window_height_ / 2);
    out_mouse_move_port_->Notify(mouse_move);

    CenteringMouse();
}

void CenteringMouseControl::CenteringMouse() const {
    sf::Mouse::setPosition(sf::Vector2i(window_width_ / 2, window_height_ / 2), window_);
}

}  // namespace null_engine
