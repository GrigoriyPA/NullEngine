#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/observer/ports.hpp>

namespace null_engine {

struct KeyboardSettings {
    float move_speed = 3.0;
    float rotation_speed = 2.0;
    float speed_ratio = 2.0;
};

class KeyboardControl {
public:
    struct CameraChange {
        float direct_move = 0.0;
        float horizon_move = 0.0;
        float vertical_move = 0.0;
        float roll_rotation = 0.0;
    };

    explicit KeyboardControl(const KeyboardSettings& settings = {});

    InPort<float>* GetRefreshPort();

    void SubscribeOnCameraChange(InPort<CameraChange>* observer_port) const;

private:
    void OnRefresh(float delta_time) const;

    KeyboardSettings settings_;
    InPort<float> in_refresh_port_;
    OutPort<CameraChange>::Uptr out_camera_change_port_ = OutPort<CameraChange>::Make();
};

}  // namespace null_engine
