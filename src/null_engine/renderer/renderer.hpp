#pragma once

#include <null_engine/renderer/camera/camera_interface.hpp>
#include <null_engine/renderer/clipping/clipping.hpp>
#include <null_engine/renderer/rasterization/rasterizer.hpp>
#include <null_engine/scene/scene.hpp>

namespace null_engine {

struct RenderEvent {
    const Scene& scene;
    AnyCameraRef camera;
};

struct RendererSettings {
    uint64_t view_width;
    uint64_t view_height;
    ClipperSettings clipper_settings = {};
};

class Renderer {
    using Clipper = detail::Clipper;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;
    using FragmentShader = detail::FragmentShader;

public:
    using TextureData = std::vector<uint8_t>;

    explicit Renderer(const RendererSettings& settings);

    InPort<RenderEvent>* GetRenderPort();

    void SubscribeToTextures(InPort<TextureData>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void RenderPointsObject(const VerticesObject& object);

    void RenderLinesObject(const VerticesObject& object);

    void RenderTrianglesObject(const VerticesObject& object);

    void ClearBuffer();

    RendererSettings settings_;
    Clipper clipper_;
    RasterizerBuffer buffer_;
    Rasterizer rasterizer_;
    FragmentShader fragment_shader_;
    ProjectiveTransform camera_transform_;
    Transform object_transform_;
    InPort<RenderEvent> in_render_port_;
    OutPort<TextureData>::Ptr out_texture_port_ = OutPort<TextureData>::Make();
};

}  // namespace null_engine
