#pragma once

#include <null_engine/renderer/camera/camera.hpp>
#include <null_engine/renderer/renderer.hpp>
#include <null_engine/scene/animations/animator.hpp>

#include "events.hpp"

namespace null_engine::tests {

struct ModelAssetes {
    std::vector<Texture::Ptr> textures;
};

class Model {
    friend class Controller;

public:
    Model(uint64_t view_width, uint64_t view_height);

    void SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const;

    void DoRendering();

private:
    void MoveCamera(const CameraChange& camera_change);

    void Refresh(FloatType delta_time);

    void OnRenderedTexture(const TextureData& texture);

    ModelAssetes assets_;
    AnimatorRegistry animator_registry_;
    PerspectiveCamera camera_;
    Scene scene_;
    Renderer renderer_;
    InPort<TextureData> in_texture_port_;
    OutPort<DrawViewEvent>::Ptr out_draw_event_port_ = OutPort<DrawViewEvent>::Make();

    TextureData current_texture_;
    FloatType current_delta_time_ = 0.0;
};

}  // namespace null_engine::tests
