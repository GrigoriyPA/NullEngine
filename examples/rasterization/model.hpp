#pragma once

#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/renderer/multithread_renderer.hpp>
#include <null_engine/renderer/native_renderer.hpp>

#include "common.hpp"
#include "scenes/example_scenes.hpp"

namespace null_engine::example {

class Model {
    friend class Controller;

    using NativeRenderer = native::Renderer;
    using MultithreadRenderer = multithread::Renderer;
    using AccelerationContext = multithread::AccelerationContext;

public:
    Model(uint64_t view_width, uint64_t view_height, MultithreadingMode multithreading_mode);

    void SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const;

    void DoRendering();

private:
    void MoveCamera(const CameraChange& camera_change);

    void Refresh(float delta_time);

    void OnNativeRenderedTexture(const TextureData& texture);

    void OnMultithreadRenderedTexture(GLuint texture_id);

    std::optional<AccelerationContext> acceleration_context_;
    SceneInfo::Uptr scene_info_;
    NativeRenderer native_renderer_;
    std::unique_ptr<MultithreadRenderer> multithread_renderer_;
    InPort<TextureData> in_texture_port_;
    InPort<GLuint> in_texture_id_port_;
    OutPort<DrawViewEvent>::Uptr out_draw_event_port_ = OutPort<DrawViewEvent>::Make();

    TextureData current_texture_;
    GLuint current_texture_id_ = 0;
    float current_delta_time_ = 0.0;
};

}  // namespace null_engine::example
