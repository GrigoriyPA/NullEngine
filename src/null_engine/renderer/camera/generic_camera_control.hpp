#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/interface/helpers/events.hpp>
#include <null_engine/util/interface/helpers/timer.hpp>

#include "generic_camera_interface.hpp"

namespace null_engine {

struct SimpleCameraControlSettings {
    FloatType sensitivity = 0.001;
    FloatType move_speed = 3.0;
    FloatType rotation_speed = 2.0;
    FloatType speed_ratio = 2.0;
};

class SimpleCameraControl : public TimerClientBase<SimpleCameraControl>, public EventsClientBase<SimpleCameraControl> {
public:
    SimpleCameraControl(
        folly::Poly<IMovableCamera&> camera, sf::RenderWindow& window, TimerProviderRef timer, EventsProviderRef events,
        const SimpleCameraControlSettings& settings = {}
    );

    void Update(FloatType delta_time);

    void OnEvent(const sf::Event& event);

private:
    void CenteringMouse() const;

private:
    const SimpleCameraControlSettings settings_;
    const folly::Poly<IMovableCamera&> camera_;
    sf::RenderWindow& window_;
    const int32_t window_width_;
    const int32_t window_height_;
    sf::Vector2i mouse_position_;
};

}  // namespace null_engine
