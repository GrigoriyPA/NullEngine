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

class RendererBase {
protected:
    using Clipper = detail::Clipper;
    using EventsPort = InPort<RenderEvent>;

public:
    RendererBase(const RendererSettings& settings, EventsPort::EventsHandler events_handler);

    EventsPort* GetRenderPort();

protected:
    uint64_t view_width_;
    uint64_t view_height_;
    Clipper clipper_;

private:
    EventsPort in_render_port_;
};

namespace native {

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

}  // namespace native

namespace multithread {

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

    compute::context context_;
    compute::command_queue queue_;
    Buffer buffer_;
    Rasterizer rasterizer_;
    Vec3 view_pos_;
    ProjectiveTransform camera_transform_;
    Transform object_transform_;
    OutPort<GLuint>::Ptr out_texture_port_ = OutPort<GLuint>::Make();
};

}  // namespace multithread

}  // namespace null_engine
