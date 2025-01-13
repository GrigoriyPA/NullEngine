#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/interface/helpers/events.hpp>
#include <null_engine/util/interface/helpers/timer.hpp>

#include "generic_camera_interface.hpp"

namespace null_engine::generic {

struct SimpleCameraControlSettings {
    util::FloatType sensitivity = 0.001;
    util::FloatType move_speed = 3.0;
    util::FloatType rotation_speed = 2.0;
    util::FloatType speed_ratio = 2.0;
};

class SimpleCameraControl : public util::TimerClientBase<SimpleCameraControl>,
                            public util::EventsClientBase<SimpleCameraControl> {
public:
    SimpleCameraControl(
        folly::Poly<IMovableCamera&> camera, sf::RenderWindow& window, util::TimerProviderRef timer,
        util::EventsProviderRef events, const SimpleCameraControlSettings& settings = {}
    );

    void Update(util::FloatType delta_time);

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
    bool initialized_ = false;
};

}  // namespace null_engine::generic
