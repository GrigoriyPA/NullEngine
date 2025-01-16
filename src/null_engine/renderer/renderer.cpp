#include "renderer.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

Renderer::Renderer(const RendererSettings& settings)
    : settings_(settings)
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

    const auto& ndc_transform = render_event.camera.GetNdcTransform();
    for (const auto& object : render_event.scene.GetObjects()) {
        if (object.IsPointsObject()) {
            RenderPointsObject(object, ndc_transform);
        } else if (object.IsTrianglesObject()) {
            RenderTrianglesObject(object, ndc_transform);
        } else {
            assert(false && "Unsupported object type for rendering");
        }
    }

    out_texture_port_->Notify(buffer_.colors);
}

void Renderer::RenderPointsObject(const VerticesObject& object, const Mat4& ndc_transform) {
    assert(object.IsPointsObject() && "Unexpected object type");

    const auto& vertices = object.GetVertices();
    for (uint64_t index : object.GetIndices()) {
        auto point = vertices[index];
        point.position = ndc_transform.Apply(point.position);

        if (!Equal(point.position.H(), 0.0)) {
            rasterizer_.DrawPoint(point, buffer_);
        }
    }
}

void Renderer::RenderTrianglesObject(const VerticesObject& object, const Mat4& ndc_transform) {
    assert(object.IsTrianglesObject() && "Unexpected object type");

    std::vector<Vertex> vertices;
    vertices.reserve(object.GetNumberVertices());
    for (const auto& vertex : object.GetVertices()) {
        vertices.emplace_back(ndc_transform.Apply(vertex.position), vertex.params);
    }

    const auto clipped = clipper_.ClipTriangles(std::move(vertices), object.GetTriangleIndices());
    for (auto [point_a, point_b, point_c] : clipped.indices) {
        rasterizer_.DrawTriangle(
            clipped.vertices[point_a], clipped.vertices[point_b], clipped.vertices[point_c], buffer_
        );
    }
}

void Renderer::ClearBuffer() {
    buffer_.colors.assign(4 * settings_.view_width * settings_.view_height, 0);
    buffer_.depth.assign(settings_.view_width * settings_.view_height, 1.0);
}

}  // namespace null_engine
