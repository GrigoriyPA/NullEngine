#include "keyboard_control.hpp"

#include <SFML/Window/Keyboard.hpp>

namespace null_engine {

KeyboardControl::KeyboardControl(const KeyboardSettings& settings)
    : settings_(settings)
    , in_refresh_port_(std::bind(&KeyboardControl::OnRefresh, this, std::placeholders::_1)) {
}

InPort<FloatType>* KeyboardControl::GetRefreshPort() {
    return &in_refresh_port_;
}

void KeyboardControl::SubscribeOnCameraChange(InPort<CameraChange>* observer_port) const {
    out_camera_change_port_->Subscribe(observer_port, {});
}

void KeyboardControl::OnRefresh(FloatType delta_time) const {
    CameraChange change;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        change.roll_rotation -= settings_.rotation_speed * delta_time;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        change.roll_rotation += settings_.rotation_speed * delta_time;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        delta_time *= settings_.speed_ratio;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        change.direct_move += settings_.move_speed * delta_time;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        change.direct_move -= settings_.move_speed * delta_time;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        change.horizon_move += settings_.move_speed * delta_time;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        change.horizon_move -= settings_.move_speed * delta_time;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        change.vertical_move += settings_.move_speed * delta_time;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
        change.vertical_move -= settings_.move_speed * delta_time;
    }

    out_camera_change_port_->Notify(change);
}

}  // namespace null_engine
