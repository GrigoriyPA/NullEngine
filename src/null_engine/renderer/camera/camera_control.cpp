#include "camera_control.hpp"

#include <SFML/Window/Keyboard.hpp>

namespace null_engine {

CameraControl::CameraControl(AnyMovableCameraRef camera, const CameraControlSettings& settings)
    : settings_(settings)
    , camera_(camera)
    , in_refresh_port_(InPort<FloatType>::Make(std::bind(&CameraControl::OnRefresh, this, std::placeholders::_1)))
    , in_mouse_move_port_(InPort<Vec2>::Make(std::bind(&CameraControl::OnMouseMove, this, std::placeholders::_1))) {
}

InPort<FloatType>* CameraControl::GetRefreshPort() const {
    return in_refresh_port_.get();
}

InPort<Vec2>* CameraControl::GetMouseMovePort() const {
    return in_mouse_move_port_.get();
}

void CameraControl::OnRefresh(FloatType delta_time) const {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        camera_->Rotate(camera_->GetDirection(), -settings_.rotation_speed * delta_time);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        camera_->Rotate(camera_->GetDirection(), settings_.rotation_speed * delta_time);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        delta_time *= settings_.speed_ratio;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        camera_->Move(settings_.move_speed * delta_time * camera_->GetDirection());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        camera_->Move(-settings_.move_speed * delta_time * camera_->GetDirection());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        camera_->Move(settings_.move_speed * delta_time * camera_->GetHorizon());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        camera_->Move(-settings_.move_speed * delta_time * camera_->GetHorizon());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        camera_->Move(settings_.move_speed * delta_time * camera_->GetVertical());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
        camera_->Move(-settings_.move_speed * delta_time * camera_->GetVertical());
    }
}

void CameraControl::OnMouseMove(Vec2 move) const {
    camera_->Rotate(camera_->GetVertical(), move.X() * settings_.sensitivity);
    camera_->Rotate(camera_->GetHorizon(), move.Y() * settings_.sensitivity);
}

}  // namespace null_engine
