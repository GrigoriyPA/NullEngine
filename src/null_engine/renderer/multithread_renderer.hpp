#pragma once

#include <null_engine/renderer/rasterization/multithread_rasterizer.hpp>

#include "common.hpp"

namespace null_engine::multithread {

class Renderer : public RendererBase {
    using Base = RendererBase;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;

    struct Buffer {
        GLuint rendering_texture = 0;
        RasterizerBuffer rasterizer_buffer;
    };

public:
    Renderer(const RendererSettings& settings, AccelerationContext context);

    void SubscribeToTextures(InPort<GLuint>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void RenderTrianglesObject(const VerticesObject& object);

    Buffer CreateBuffer();

    void ClearBuffer();

    cl_int2 view_size_;
    compute::context context_;
    compute::command_queue queue_;
    compute::program clear_buffer_program_;
    compute::kernel clear_buffer_kernel_;
    Buffer buffer_;
    Rasterizer rasterizer_;
    Vec3 view_pos_;
    ProjectiveTransform camera_transform_;
    Transform object_transform_;
    OutPort<GLuint>::Ptr out_texture_port_ = OutPort<GLuint>::Make();
};

}  // namespace null_engine::multithread
