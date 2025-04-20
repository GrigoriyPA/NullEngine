#pragma once

#include <null_engine/acceleration/kernel.hpp>
#include <null_engine/renderer/rasterization/multithread_rasterizer.hpp>

#include "common.hpp"

namespace null_engine::multithread {

class Renderer : public RendererBase {
    using Base = RendererBase;
    using Kernel = detail::Kernel;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;
    using FragmentShader = detail::FragmentShader;

    struct Buffer {
        GLuint rendering_texture = 0;
        RasterizerBuffer rasterizer_buffer;
    };

    class CleanupKernel {
        using Program = detail::Program;

        enum KernelArgs {
            KA_VIEW_SIZE,
            KA_VIEW,
            KA_DEPTH,
            KA_COLOR,
        };

        static constexpr cl_int2 kLocalSize = {.x = 256, .y = 1};

    public:
        CleanupKernel(const RendererSettings& settings, const Buffer& buffer, AccelerationContext context);

        static Program GetProgram();

        void Run();

    private:
        cl_int2 view_size_;
        Kernel kernel_;
    };

public:
    Renderer(const RendererSettings& settings, AccelerationContext context);

    void SubscribeToTextures(InPort<GLuint>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void RenderTrianglesObject(const VerticesObject& object);

    Buffer CreateBuffer();

    AccelerationContext context_;
    Buffer buffer_;
    CleanupKernel clear_buffer_kernel_;
    FragmentShader fragment_shader_;
    Rasterizer rasterizer_;
    Vec3 view_pos_;
    ProjectiveTransform camera_transform_;
    Transform object_transform_;
    OutPort<GLuint>::Ptr out_texture_port_ = OutPort<GLuint>::Make();
};

}  // namespace null_engine::multithread
