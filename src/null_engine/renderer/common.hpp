#pragma once

#include <null_engine/renderer/camera/camera_interface.hpp>
#include <null_engine/renderer/clipping/clipping.hpp>
#include <null_engine/scene/scene.hpp>

namespace null_engine {

struct RenderEvent {
    const Scene& scene;
    AnyCameraRef camera;
};

struct RendererSettings {
    uint64_t view_width;
    uint64_t view_height;
    Vec3 background_color = kBlack;
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
    Vec3 background_color_;
    Clipper clipper_;

private:
    EventsPort in_render_port_;
};

}  // namespace null_engine
