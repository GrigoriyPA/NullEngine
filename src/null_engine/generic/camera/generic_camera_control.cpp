#include "generic_camera_control.hpp"

#include <null_engine/util/geometry/constants.hpp>

namespace null_engine::generic {

SimpleCameraControl::SimpleCameraControl(
    folly::Poly<IMovableCamera&> camera, sf::RenderWindow& window, util::TimerProviderRef timer,
    util::EventsProviderRef events, const SimpleCameraControlSettings& settings
)
    : util::TimerClientBase<SimpleCameraControl>(timer)
    , util::EventsClientBase<SimpleCameraControl>(events)
    , settings_(settings)
    , camera_(camera)
    , window_(window)
    , window_width_(static_cast<int32_t>(window_.getSize().x))
    , window_height_(static_cast<int32_t>(window_.getSize().y))
    , mouse_position_(sf::Mouse::getPosition()) {
    window_.setMouseCursorVisible(false);
    CenteringMouse();
}

void SimpleCameraControl::Update(util::FloatType delta_time) {
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

void SimpleCameraControl::OnEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseMoved) {
        return;
    }

    if (!initialized_) {
        initialized_ = true;
        return;
    }

    camera_->Rotate(
        camera_->GetVertical(),
        static_cast<util::FloatType>(event.mouseMove.x - window_width_ / 2) * settings_.sensitivity
    );

    camera_->Rotate(
        camera_->GetHorizon(),
        static_cast<util::FloatType>(event.mouseMove.y - window_height_ / 2) * settings_.sensitivity
    );

    CenteringMouse();
}

void SimpleCameraControl::CenteringMouse() const {
    sf::Mouse::setPosition(sf::Vector2i(window_width_ / 2, window_height_ / 2), window_);
}

}  // namespace null_engine::generic
