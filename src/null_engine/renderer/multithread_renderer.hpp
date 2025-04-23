#pragma once

#include <null_engine/acceleration/buffer.hpp>
#include <null_engine/acceleration/kernel.hpp>
#include <null_engine/renderer/rasterization/multithread_rasterizer.hpp>
#include <null_engine/renderer/shaders/multithread_fragment_shader.hpp>

#include "common.hpp"

namespace null_engine::multithread {

class Renderer : public RendererBase {
    using Base = RendererBase;
    using Kernel = detail::Kernel;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;
    using MainFragmentShader = detail::MainFragmentShader;
    using NoopFragmentShader = detail::NoopFragmentShader;
    using AnyFragmentShaderRef = detail::AnyFragmentShaderRef;
    using ShadowsBuffer = detail::DynamicBuffer<cl_float>;

    struct Buffer {
        GLuint rendering_texture = 0;
        RasterizerBuffer rasterizer_buffer;
    };

    class CleanupMainKernel {
        using Program = detail::Program;

        enum KernelArgs {
            KA_VIEW_SIZE,
            KA_VIEW,
            KA_DEPTH,
            KA_COLOR,
        };

        static constexpr cl_int2 kLocalSize = {.x = 256, .y = 1};

    public:
        CleanupMainKernel(const RendererSettings& settings, const Buffer& buffer, AccelerationContext context);

        static Program GetProgram();

        void Run();

    private:
        cl_int2 view_size_;
        Kernel kernel_;
    };

    class CleanupDepthKernel {
        using Program = detail::Program;

        enum KernelArgs {
            KA_SIZE,
            KA_DEPTH,
        };

        static constexpr cl_int kLocalSize = 256;

    public:
        explicit CleanupDepthKernel(AccelerationContext context);

        static Program GetProgram();

        void Run(cl_int size, const compute::buffer& buffer);

    private:
        Kernel kernel_;
    };

public:
    Renderer(const RendererSettings& settings, AccelerationContext context);

    void SubscribeToTextures(InPort<GLuint>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void FillShadowsMap(const Scene& scene);

    void RenderScene(const Scene& scene, AnyCameraRef camera);

    struct RenderingContext {
        Rasterizer& rasterizer;
        RasterizerBuffer& buffer;
        Vec3 view_pos;
        ProjectiveTransform camera_transform;
        Transform object_transform;
    };

    void RenderObject(const VerticesObject& object, const std::vector<Transform>& instances, RenderingContext& context);

    void RenderTrianglesObject(const VerticesObject& object, RenderingContext& context);

    Buffer CreateBuffer();

    AccelerationContext context_;
    Buffer main_buffer_;
    CleanupMainKernel clear_main_buffer_kernel_;
    CleanupDepthKernel clear_shadow_map_kernel_;
    MainFragmentShader main_fragment_shader_;
    NoopFragmentShader depth_fragment_shader_;
    ShadowsBuffer shadows_map_;
    MainFragmentShader::ShadowsMaps shadows_info_;
    Rasterizer main_rasterizer_;
    Rasterizer depth_rasterizer_;
    OutPort<GLuint>::Ptr out_texture_port_ = OutPort<GLuint>::Make();
};

}  // namespace null_engine::multithread
