#pragma once

#include <null_engine/renderer/renderer.hpp>

#include "events.hpp"

namespace null_engine::tests {

class Model {
    friend class Controller;

    struct CameraChange {
        struct Move {
            FloatType direct_move = 0.0;
            FloatType horizon_move = 0.0;
            FloatType vertical_move = 0.0;
        };

        struct Rotation {
            FloatType yaw_rotation = 0.0;
            FloatType pitch_rotation = 0.0;
            FloatType roll_rotation = 0.0;
        };

        Move move;
        Rotation rotate;
    };

public:
    Model(uint64_t view_width, uint64_t view_height);

    void SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const;

    void DoRendering();

private:
    void MoveCamera(const CameraChange& camera_change);

    void Refresh(FloatType delta_time);

    void OnRenderedTexture(const TextureData& texture);

    Scene scene_;
    AnyMovableCamera camera_;
    Renderer renderer_;
    InPort<TextureData> in_texture_port_;
    OutPort<DrawViewEvent>::Ptr out_draw_event_port_ = OutPort<DrawViewEvent>::Make();

    TextureData current_texture_;
    FloatType current_delta_time_ = 0.0;
};

}  // namespace null_engine::tests
