#include "native_renderer.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

using namespace detail;

namespace native {

Renderer::Renderer(const RendererSettings& settings)
    : Base(settings, std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))
    , rasterizer_(settings.view_width, settings.view_height) {
}

void Renderer::SubscribeToTextures(InPort<TextureData>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, buffer_.colors);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    ClearBuffer();

    fragment_shader_.SetViewPos(render_event.camera->GetViewPos());
    fragment_shader_.SetLights(render_event.scene.GetLights());
    camera_transform_ = render_event.camera->GetNdcTransform();

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

    const auto& vertices = ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices());
    for (uint64_t index : object.GetIndices()) {
        const auto& point = vertices[index];
        if (!Equal(point.position.w(), 0.0)) {
            rasterizer_.DrawPoint(point, buffer_, fragment_shader_);
        }
    }
}

void Renderer::RenderLinesObject(const VerticesObject& object) {
    assert(object.IsLinesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipLines(
        ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices()), object.GetLinesIndices()
    );

    for (auto [point_a, point_b] : clipped.indices) {
        rasterizer_.DrawLine(clipped.vertices[point_a], clipped.vertices[point_b], buffer_, fragment_shader_);
    }
}

void Renderer::RenderTrianglesObject(const VerticesObject& object) {
    assert(object.IsTrianglesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipTriangles(
        fragment_shader_.GetViewPos(), ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices()),
        object.GetTriangleIndices()
    );

    for (auto [point_a, point_b, point_c] : clipped.indices) {
        rasterizer_.DrawTriangle(
            clipped.vertices[point_a], clipped.vertices[point_b], clipped.vertices[point_c], buffer_, fragment_shader_
        );
    }
}

void Renderer::ClearBuffer() {
    buffer_.colors.resize(4 * view_width_ * view_height_);
    for (size_t i = 0; i < 4 * view_width_ * view_height_; i += 4) {
        buffer_.colors[i] = static_cast<uint8_t>(background_color_.x());
        buffer_.colors[i + 1] = static_cast<uint8_t>(background_color_.y());
        buffer_.colors[i + 2] = static_cast<uint8_t>(background_color_.z());
        buffer_.colors[i + 3] = 255;
    }

    buffer_.depth.assign(view_width_ * view_height_, 1.0);
}

}  // namespace native

}  // namespace null_engine
