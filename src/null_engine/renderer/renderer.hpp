#pragma once

#include <null_engine/renderer/camera/camera_interface.hpp>
#include <null_engine/renderer/clipping/clipping.hpp>
#include <null_engine/renderer/rasterization/rasterizer.hpp>
#include <null_engine/scene/scene.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

struct RenderEvent {
    const Scene& scene;
    const AnyCamera& camera;
};

struct RendererSettings {
    uint64_t view_width;
    uint64_t view_height;
};

class Renderer {
    using Clipper = detail::Clipper;
    using RasterizerBuffer = detail::RasterizerBuffer;
    using Rasterizer = detail::Rasterizer;

public:
    using TextureData = std::vector<uint8_t>;

    explicit Renderer(const RendererSettings& settings);

    InPort<RenderEvent>* GetRenderPort();

    void SubscribeToTextures(InPort<TextureData>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void RenderPointsObject(const VerticesObject& object, const Mat4& ndc_transform);

    void RenderLinesObject(const VerticesObject& object, const Mat4& ndc_transform);

    void RenderTrianglesObject(const VerticesObject& object, const Mat4& ndc_transform);

    void ClearBuffer();

    static std::vector<Vertex> TransformObjectVertices(const VerticesObject& object, const Mat4& ndc_transform);

    RendererSettings settings_;
    Clipper clipper_;
    RasterizerBuffer buffer_;
    Rasterizer rasterizer_;
    InPort<RenderEvent> in_render_port_;
    OutPort<TextureData>::Ptr out_texture_port_ = OutPort<TextureData>::Make();
};

}  // namespace null_engine
