#pragma once

#include <null_engine/renderer/camera/camera_interface.hpp>
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
public:
    using TextureData = std::vector<uint8_t>;

    explicit Renderer(const RendererSettings& settings);

    InPort<RenderEvent>* GetRenderPort() const;

    void SubscribeToTextures(InPort<TextureData>* observer_port) const;

private:
    void OnRenderEvent(const RenderEvent& render_event);

    void ClearBuffer();

    RendererSettings settings_;
    RasterizerBuffer buffer_;
    Rasterizer rasterizer_;
    InPort<RenderEvent>::Ptr in_render_port_ = InPort<RenderEvent>::Make();
    OutPort<TextureData>::Ptr out_texture_port_ = OutPort<TextureData>::Make();
};

}  // namespace null_engine
