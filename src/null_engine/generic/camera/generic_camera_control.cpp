#include <null_engine/generic/camera/generic_camera_control.hpp>

#include <null_engine/util/geometry/constants.hpp>

namespace null_engine::generic {

//// SimpleCameraControl

SimpleCameraControl::SimpleCameraControl(
    MovableCamera::Ptr camera, util::WindowPtr window, const SimpleCameraControlSettings& settings
)
    : settings_(settings)
    , camera_(camera)
    , window_(window)
    , window_width_(static_cast<int32_t>(window_->getSize().x))
    , window_height_(static_cast<int32_t>(window_->getSize().y))
    , mouse_position_(sf::Mouse::getPosition()) {
    window->setMouseCursorVisible(false);
    CenteringMouse();
}

void SimpleCameraControl::Update(util::FloatType delta_time) {
    // Roll rotation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        camera_->Rotate(camera_->GetDirection(), -settings_.rotation_speed * delta_time);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        camera_->Rotate(camera_->GetDirection(), settings_.rotation_speed * delta_time);
    }

    // Camera acceleration
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        delta_time *= settings_.speed_ratio;
    }

    // Camera forward moving
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        camera_->Move(settings_.move_speed * delta_time * camera_->GetDirection());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        camera_->Move(-settings_.move_speed * delta_time * camera_->GetDirection());
    }

    // Camera horizon moving
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        camera_->Move(settings_.move_speed * delta_time * camera_->GetHorizon());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        camera_->Move(-settings_.move_speed * delta_time * camera_->GetHorizon());
    }

    // Camera vertical moving
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        camera_->Move(settings_.move_speed * delta_time * camera_->GetVertical());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
        camera_->Move(-settings_.move_speed * delta_time * camera_->GetVertical());
    }
}

void SimpleCameraControl::OnEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseMoved) {
        return;
    }

    if (!initialized_) {
        // Skip first mouse event due to mouse centering
        initialized_ = true;
        return;
    }

    // Yaw rotation
    camera_->Rotate(
        camera_->GetVertical(),
        static_cast<util::FloatType>(event.mouseMove.x - window_width_ / 2) * settings_.sensitivity
    );

    // Pitch rotation
    camera_->Rotate(
        camera_->GetHorizon(),
        static_cast<util::FloatType>(event.mouseMove.y - window_height_ / 2) * settings_.sensitivity
    );

    CenteringMouse();
}

void SimpleCameraControl::CenteringMouse() const {
    sf::Mouse::setPosition(sf::Vector2i(window_width_ / 2, window_height_ / 2), *window_);
}

}  // namespace null_engine::generic
