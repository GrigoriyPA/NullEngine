#pragma once

#include <null_engine/util/interface/controls/centering_mouse_control.hpp>
#include <null_engine/util/interface/controls/keyboard_control.hpp>

#include "model.hpp"

namespace null_engine::tests {

class Controller {
    using MouseChange = CenteringMouseControl::CameraChange;
    using KeyboardChange = KeyboardControl::CameraChange;

public:
    Controller(sf::RenderWindow& window, Model* model);

    void AddEvent(const sf::Event& event);

    void AddRefresh(FloatType delta_time);

    void Apply();

private:
    void OnMouseChange(const MouseChange& change) const;

    void OnKeyboardChange(const KeyboardChange& change) const;

    Model* model_;
    CenteringMouseControl mouse_control_;
    KeyboardControl keyboard_control_;
    InPort<MouseChange> in_mouse_change_port_;
    InPort<KeyboardChange> in_keyboard_change_port_;
};

}  // namespace null_engine::tests
