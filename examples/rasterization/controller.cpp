#include "controller.hpp"

namespace null_engine::tests {

Controller::Controller(sf::RenderWindow& window, Model* model)
    : model_(model)
    , mouse_control_(window)
    , keyboard_control_({})
    , in_mouse_change_port_(std::bind(&Controller::OnMouseChange, this, std::placeholders::_1))
    , in_keyboard_change_port_(std::bind(&Controller::OnKeyboardChange, this, std::placeholders::_1)) {
    mouse_control_.SubscribeOnCameraChange(&in_mouse_change_port_);
    keyboard_control_.SubscribeOnCameraChange(&in_keyboard_change_port_);
}

void Controller::AddEvent(const sf::Event& event) {
    mouse_control_.GetEventsPort()->OnEvent(event);
}

void Controller::AddRefresh(FloatType delta_time) {
    keyboard_control_.GetRefreshPort()->OnEvent(delta_time);
    model_->Refresh(delta_time);
}

void Controller::Apply() {
    model_->DoRendering();
}

void Controller::OnMouseChange(const MouseChange& change) const {
    model_->MoveCamera({.rotate = {.yaw = change.yaw_rotation, .pitch = change.pitch_rotation}});
}

void Controller::OnKeyboardChange(const KeyboardChange& change) const {
    model_->MoveCamera({
        .move = {.direct = change.direct_move, .horizon = change.horizon_move, .vertical = change.vertical_move},
        .rotate = {.roll = change.roll_rotation},
    });
}

}  // namespace null_engine::tests
