#include "common.hpp"

namespace null_engine {

RendererBase::RendererBase(const RendererSettings& settings, EventsPort::EventsHandler events_handler)
    : view_width_(settings.view_width)
    , view_height_(settings.view_height)
    , background_color_((settings.background_color * 255.0).cwiseMax(0.0).cwiseMin(255.0))
    , clipper_(settings.clipper_settings)
    , in_render_port_(std::move(events_handler)) {
}

RendererBase::EventsPort* RendererBase::GetRenderPort() {
    return &in_render_port_;
}

}  // namespace null_engine
