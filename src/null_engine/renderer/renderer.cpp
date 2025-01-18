#include "renderer.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

Renderer::Renderer(const RendererSettings& settings)
    : settings_(settings)
    , clipper_(settings.clipper_settings)
    , rasterizer_(settings.view_width, settings.view_height)
    , in_render_port_(std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1)) {
}

InPort<RenderEvent>* Renderer::GetRenderPort() {
    return &in_render_port_;
}

void Renderer::SubscribeToTextures(InPort<TextureData>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, buffer_.colors);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    ClearBuffer();

    fragment_shader_.SetViewPos(render_event.camera.GetViewPos());
    camera_transform_ = render_event.camera.GetNdcTransform();

    for (const auto& [object, instances] : render_event.scene) {
        fragment_shader_.SetMaterial(object.GetMaterial());

        for (const auto& instance_transform : instances) {
            object_transform_ = instance_transform;

            if (object.IsPointsObject()) {
                RenderPointsObject(object);
            } else if (object.IsLinesObject()) {
                RenderLinesObject(object);
            } else if (object.IsTrianglesObject()) {
                RenderTrianglesObject(object);
            } else {
                assert(false && "Unsupported object type for rendering");
            }
        }
    }

    out_texture_port_->Notify(buffer_.colors);
}

void Renderer::RenderPointsObject(const VerticesObject& object) {
    assert(object.IsPointsObject() && "Unexpected object type");

    const auto& vertices = detail::ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices());
    for (uint64_t index : object.GetIndices()) {
        const auto& point = vertices[index];
        if (!Equal(point.position.H(), 0.0)) {
            rasterizer_.DrawPoint(point, buffer_, fragment_shader_);
        }
    }
}

void Renderer::RenderLinesObject(const VerticesObject& object) {
    assert(object.IsLinesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipLines(
        detail::ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices()),
        object.GetLinesIndices()
    );

    for (auto [point_a, point_b] : clipped.indices) {
        rasterizer_.DrawLine(clipped.vertices[point_a], clipped.vertices[point_b], buffer_, fragment_shader_);
    }
}

void Renderer::RenderTrianglesObject(const VerticesObject& object) {
    assert(object.IsTrianglesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipTriangles(
        fragment_shader_.GetViewPos(),
        detail::ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices()),
        object.GetTriangleIndices()
    );

    for (auto [point_a, point_b, point_c] : clipped.indices) {
        rasterizer_.DrawTriangle(
            clipped.vertices[point_a], clipped.vertices[point_b], clipped.vertices[point_c], buffer_, fragment_shader_
        );
    }
}

void Renderer::ClearBuffer() {
    buffer_.colors.assign(4 * settings_.view_width * settings_.view_height, 0);
    buffer_.depth.assign(settings_.view_width * settings_.view_height, 1.0);
}

}  // namespace null_engine
