#include "centering_mouse_control.hpp"

#include <SFML/Window/Event.hpp>

namespace null_engine {

CenteringMouseControl::CenteringMouseControl(sf::RenderWindow& window, const CenteringMouseSettings& settings)
    : window_(window)
    , settings_(settings)
    , window_width_(window_.getSize().x)
    , window_height_(window_.getSize().y)
    , in_events_port_(std::bind(&CenteringMouseControl::OnEvent, this, std::placeholders::_1)) {
    window_.setMouseCursorVisible(false);
    CenteringMouse();
}

InPort<sf::Event>* CenteringMouseControl::GetEventsPort() {
    return &in_events_port_;
}

void CenteringMouseControl::SubscribeOnCameraChange(InPort<CameraChange>* observer_port) const {
    out_camera_change_port_->Subscribe(observer_port, {});
}

void CenteringMouseControl::OnEvent(const sf::Event& event) const {
    if (event.type != sf::Event::MouseMoved) {
        return;
    }

    out_camera_change_port_->Notify(
        {.yaw_rotation = static_cast<FloatType>(event.mouseMove.x - window_width_ / 2) * settings_.sensitivity,
         .pitch_rotation = static_cast<FloatType>(event.mouseMove.y - window_height_ / 2) * settings_.sensitivity}
    );

    CenteringMouse();
}

void CenteringMouseControl::CenteringMouse() const {
    sf::Mouse::setPosition(sf::Vector2i(window_width_ / 2, window_height_ / 2), window_);
}

}  // namespace null_engine
