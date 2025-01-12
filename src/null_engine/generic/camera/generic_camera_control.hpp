#pragma once

#include "generic_camera.hpp"

#include <null_engine/util/generic/templates.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/interface/helpers/window.hpp>
#include <null_engine/util/interface/objects/interface_object.hpp>

namespace null_engine::generic {

//
// Settings for camera control:
// -> sensitivity -- mouse sensitivity (pixel speed to angle speed for pitch, yaw)
// -> move_speed -- speed relative to scene coordinates
// -> rotation_speed -- camera roll speed in radians per second
// -> speed_ratio -- camera acceleration ratio
//
struct SimpleCameraControlSettings {
    util::FloatType sensitivity = 0.001;
    util::FloatType move_speed = 3.0;
    util::FloatType rotation_speed = 2.0;
    util::FloatType speed_ratio = 2.0;
};

//
// Object which perform camera control by SFML events.
// -> 'W', 'A', 'S', 'D', 'LeftAlt', 'Space' -- to move camera
// -> 'LeftShift' -- to accelerate camera
//
class SimpleCameraControl : public util::InterfaceObject, public util::SharedConstructable<SimpleCameraControl> {
public:
    using Ptr = std::shared_ptr<SimpleCameraControl>;

public:
    explicit SimpleCameraControl(
        MovableCamera::Ptr camera, util::WindowPtr window, const SimpleCameraControlSettings& settings = {}
    );

public:
    void Update(util::FloatType delta_time) override;
    void OnEvent(const sf::Event& event) override;

private:
    void CenteringMouse() const;

private:
    const SimpleCameraControlSettings settings_;
    const MovableCamera::Ptr camera_;

    const util::WindowPtr window_;
    const int32_t window_width_;
    const int32_t window_height_;

    sf::Vector2i mouse_position_;
    bool initialized_ = false;  // Skip first mouse event due to mouse centering
};

}  // namespace null_engine::generic
