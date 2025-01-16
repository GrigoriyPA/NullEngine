#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

struct KeyboardSettings {
    FloatType move_speed = 3.0;
    FloatType rotation_speed = 2.0;
    FloatType speed_ratio = 2.0;
};

class KeyboardControl {
public:
    struct CameraChange {
        FloatType direct_move = 0.0;
        FloatType horizon_move = 0.0;
        FloatType vertical_move = 0.0;
        FloatType roll_rotation = 0.0;
    };

    explicit KeyboardControl(const KeyboardSettings& settings = {});

    InPort<FloatType>* GetRefreshPort();

    void SubscribeOnCameraChange(InPort<CameraChange>* observer_port) const;

private:
    void OnRefresh(FloatType delta_time) const;

    KeyboardSettings settings_;
    InPort<FloatType> in_refresh_port_;
    OutPort<CameraChange>::Ptr out_camera_change_port_ = OutPort<CameraChange>::Make();
};

}  // namespace null_engine
