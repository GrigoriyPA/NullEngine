#pragma once

#include <null_engine/renderer/rasterization/native_rasterizer.hpp>
#include <null_engine/renderer/shaders/native_fragment_shader.hpp>

#include "common.hpp"

namespace null_engine::native {

class Renderer : public RendererBase {
    using Base = RendererBase;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;
    using LightSettings = detail::LightSettings;
    using AnyFragmentShaderRef = detail::AnyFragmentShaderRef;
    using DepthFragmentShader = detail::NoopFragmentShader;
    using MainFragmentShader = detail::MainFragmentShader;

public:
    using TextureData = std::vector<uint8_t>;

    explicit Renderer(const RendererSettings& settings);

    void SubscribeToTextures(InPort<TextureData>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void FillLightsInfo(const Scene& scene);

    void RenderScene(const Scene& scene, AnyCameraRef camera);

    struct RenderingContext {
        AnyFragmentShaderRef fragment_shader;
        RasterizerBuffer& buffer;
        Vec3 view_pos;
        ProjectiveTransform camera_transform;
        Transform object_transform;
    };

    void RenderObject(const VerticesObject& object, const std::vector<Transform>& instances, RenderingContext& context);

    void RenderPointsObject(const VerticesObject& object, RenderingContext& context);

    void RenderLinesObject(const VerticesObject& object, RenderingContext& context);

    void RenderTrianglesObject(const VerticesObject& object, RenderingContext& context);

    void ClearBuffer();

    Vec3 background_color_;
    Rasterizer::ViewInfo view_;
    RasterizerBuffer main_buffer_;
    std::array<RasterizerBuffer, MainFragmentShader::kMaxNumberLights> depth_buffers_;
    std::vector<LightSettings> lights_info_;
    Rasterizer rasterizer_;
    DepthFragmentShader depth_fragment_shader_;
    MainFragmentShader main_fragment_shader_;
    OutPort<TextureData>::Uptr out_texture_port_ = OutPort<TextureData>::Make();
};

}  // namespace null_engine::native
