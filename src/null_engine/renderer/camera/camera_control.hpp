#pragma once

#include <null_engine/util/mvc/ports.hpp>

#include "camera_interface.hpp"

namespace null_engine {

struct CameraControlSettings {
    FloatType sensitivity = 0.001;
    FloatType move_speed = 3.0;
    FloatType rotation_speed = 2.0;
    FloatType speed_ratio = 2.0;
};

class CameraControl {
public:
    explicit CameraControl(AnyMovableCameraRef camera, const CameraControlSettings& settings = {});

    InPort<FloatType>* GetRefreshPort() const;

    InPort<Vec2>* GetMouseMovePort() const;

private:
    void OnRefresh(FloatType delta_time) const;

    void OnMouseMove(Vec2 move) const;

    CameraControlSettings settings_;
    AnyMovableCameraRef camera_;
    InPort<FloatType>::Ptr in_refresh_port_;
    InPort<Vec2>::Ptr in_mouse_move_port_;
};

}  // namespace null_engine
