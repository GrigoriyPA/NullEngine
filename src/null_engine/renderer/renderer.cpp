#include "renderer.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

namespace {

bool ApplyNdcTransform(Vec3& position, const Mat4& ndc_transform) {
    position = ndc_transform.Apply(position);

    const auto h = position.H();
    if (Equal(h, 0.0)) {
        return false;
    }

    position /= position.H();
    position.H() = 1.0 / position.H();
    return true;
}

}  // anonymous namespace

Renderer::Renderer(const RendererSettings& settings)
    : settings_(settings)
    , rasterizer_(settings.view_width, settings.view_height)
    , in_render_port_(InPort<RenderEvent>::Make(std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))) {
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
        switch (object.GetObjectType()) {
            case VerticesObject::Type::Points:
                RenderPointsObject(object, ndc_transform);
                break;

            case VerticesObject::Type::Triangles:
                RenderTrianglesObject(object, ndc_transform);
                break;
        }
    }

    out_texture_port_->Notify(buffer_.colors);
}

void Renderer::RenderPointsObject(const VerticesObject& object, const Mat4& ndc_transform) {
    assert(object.GetObjectType() == VerticesObject::Type::Points && "Unexpected object type");

    const auto& vertices = object.GetVertices();
    for (uint64_t index : object.GetIndices()) {
        auto point = vertices[index];
        if (!ApplyNdcTransform(point.position, ndc_transform)) {
            continue;
        }

        rasterizer_.DrawPoint(point, buffer_);
    }
}

void Renderer::RenderTrianglesObject(const VerticesObject& object, const Mat4& ndc_transform) {
    assert(object.GetObjectType() == VerticesObject::Type::Triangles && "Unexpected object type");

    const auto& vertices = object.GetVertices();
    const auto& indices = object.GetIndices();
    assert(indices.size() % 3 == 0 && "Unexpected number of indices for triangles object");

    for (uint64_t i = 0; i < indices.size(); i += 3) {
        auto point_a = vertices[indices[i]];
        if (!ApplyNdcTransform(point_a.position, ndc_transform)) {
            continue;
        }

        auto point_b = vertices[indices[i + 1]];
        if (!ApplyNdcTransform(point_b.position, ndc_transform)) {
            continue;
        }

        auto point_c = vertices[indices[i + 2]];
        if (!ApplyNdcTransform(point_c.position, ndc_transform)) {
            continue;
        }

        rasterizer_.DrawTriangle(point_a, point_b, point_c, buffer_);
    }
}

void Renderer::ClearBuffer() {
    buffer_.colors.assign(4 * settings_.view_width * settings_.view_height, 0);
    buffer_.depth.assign(settings_.view_width * settings_.view_height, 1.0);
}

}  // namespace null_engine
