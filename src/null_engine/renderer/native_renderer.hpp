#pragma once

#include <null_engine/renderer/rasterization/native_rasterizer.hpp>

#include "common.hpp"

namespace null_engine::native {

class Renderer : public RendererBase {
    using Base = RendererBase;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;
    using FragmentShader = detail::FragmentShader;

public:
    using TextureData = std::vector<uint8_t>;

    explicit Renderer(const RendererSettings& settings);

    void SubscribeToTextures(InPort<TextureData>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void RenderPointsObject(const VerticesObject& object);

    void RenderLinesObject(const VerticesObject& object);

    void RenderTrianglesObject(const VerticesObject& object);

    void ClearBuffer();

    RasterizerBuffer buffer_;
    Rasterizer rasterizer_;
    FragmentShader fragment_shader_;
    ProjectiveTransform camera_transform_;
    Transform object_transform_;
    OutPort<TextureData>::Ptr out_texture_port_ = OutPort<TextureData>::Make();
};

}  // namespace null_engine::native
