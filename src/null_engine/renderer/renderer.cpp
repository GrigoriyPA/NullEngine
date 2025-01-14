#include "renderer.hpp"

namespace null_engine {

namespace {

void ApplyNdcMat4(Vec3& position, const Mat4& ndc_transform) {
    position = ndc_transform.Apply(position);
    position /= position.GetH();
    position.H() = 1.0 / position.GetH();
}

}  // anonymous namespace

Renderer::Renderer(const RendererSettings& settings)
    : settings_(settings)
    , rasterizer_(settings.view_width, settings.view_height) {
    in_render_port_->SetEventsHandler(std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1));
}

InPort<RenderEvent>* Renderer::GetRenderPort() const {
    return in_render_port_.get();
}

void Renderer::SubscribeToTextures(InPort<TextureData>* observer_port) const {
    out_texture_port_->Subscribe(observer_port);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    ClearBuffer();

    const auto& ndc_transform = render_event.camera.GetNdcMat4();
    for (const auto& object : render_event.scene.GetObjects()) {
        const auto& vertices = object.GetVertices();
        for (uint64_t index : object.GetIndices()) {
            auto vertex = vertices[index];
            ApplyNdcMat4(vertex.position, ndc_transform);

            rasterizer_.DrawPoint(vertex, buffer_);
        }
    }

    out_texture_port_->Notify(buffer_.colors);
}

void Renderer::ClearBuffer() {
    buffer_.colors.assign(4 * settings_.view_width * settings_.view_height, 0);
    buffer_.depth.assign(settings_.view_width * settings_.view_height, 1.0);
}

}  // namespace null_engine
