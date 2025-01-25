#pragma once

#include <null_engine/acceleration/acceleration_context.hpp>
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

    using NativeRenderer = native::Renderer;
    using MultithreadRenderer = multithread::Renderer;
    using AccelerationContext = multithread::AccelerationContext;

public:
    Model(uint64_t view_width, uint64_t view_height, bool multithread_rendering);

    void SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const;

    void DoRendering();

private:
    void MoveCamera(const CameraChange& camera_change);

    void Refresh(FloatType delta_time);

    void OnNativeRenderedTexture(const TextureData& texture);

    void OnMultithreadRenderedTexture(GLuint texture_id);

    ModelAssetes assets_;
    AccelerationContext acceleration_context_;
    AnimatorRegistry animator_registry_;
    PerspectiveCamera camera_;
    Scene scene_;
    NativeRenderer native_renderer_;
    MultithreadRenderer multithread_renderer_;
    bool multithread_rendering_;
    InPort<TextureData> in_texture_port_;
    InPort<GLuint> in_texture_id_port_;
    OutPort<DrawViewEvent>::Ptr out_draw_event_port_ = OutPort<DrawViewEvent>::Make();

    TextureData current_texture_;
    GLuint current_texture_id_ = 0;
    FloatType current_delta_time_ = 0.0;
};

}  // namespace null_engine::tests
