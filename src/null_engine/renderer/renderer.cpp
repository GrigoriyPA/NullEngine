#include "renderer.hpp"

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/acquire.hpp>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

using namespace detail;

RendererBase::RendererBase(const RendererSettings& settings, EventsPort::EventsHandler events_handler)
    : view_width_(settings.view_width)
    , view_height_(settings.view_height)
    , clipper_(settings.clipper_settings)
    , in_render_port_(std::move(events_handler)) {
}

RendererBase::EventsPort* RendererBase::GetRenderPort() {
    return &in_render_port_;
}

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
    buffer_.colors.assign(4 * view_width_ * view_height_, 0);
    buffer_.depth.assign(view_width_ * view_height_, 1.0);
}

}  // namespace native

namespace multithread {

Renderer::Renderer(const RendererSettings& settings, AccelerationContext context)
    : Base(settings, std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , buffer_(CreateBuffer())
    , rasterizer_(view_width_, view_height_, context_) {
}

void Renderer::SubscribeToTextures(InPort<GLuint>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, buffer_.rendering_texture);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    compute::opengl_enqueue_acquire_gl_objects(1, &buffer_.rasterizer_buffer.colors.get(), queue_);

    view_pos_ = render_event.camera->GetViewPos();
    camera_transform_ = render_event.camera->GetNdcTransform();

    for (const auto& [object, instances] : render_event.scene) {
        for (const auto& instance_transform : instances) {
            object_transform_ = instance_transform;

            if (object.IsTrianglesObject()) {
                RenderTrianglesObject(object);
            } else {
                assert(false && "Unsupported object type for rendering");
            }
        }
    }

    compute::opengl_enqueue_release_gl_objects(1, &buffer_.rasterizer_buffer.colors.get(), queue_);
    queue_.finish();

    out_texture_port_->Notify(buffer_.rendering_texture);
}

void Renderer::RenderTrianglesObject(const VerticesObject& object) {
    assert(object.IsTrianglesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipTriangles(
        view_pos_, ConvertObjectVerices(camera_transform_, object_transform_, object.GetVertices()),
        object.GetTriangleIndices()
    );

    rasterizer_.DrawTriangles(clipped.vertices, clipped.indices, buffer_.rasterizer_buffer);
}

Renderer::Buffer Renderer::CreateBuffer() {
    Buffer result;

    glGenTextures(1, &result.rendering_texture);
    glBindTexture(GL_TEXTURE_2D, result.rendering_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view_width_, view_height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    result.rasterizer_buffer = {
        .colors = compute::opengl_texture(context_, GL_TEXTURE_2D, 0, result.rendering_texture, CL_MEM_WRITE_ONLY),
        .depth = compute::buffer(context_, view_width_ * view_height_ * sizeof(FloatType)),
        .queue = queue_,
    };

    return result;
}

}  // namespace multithread

}  // namespace null_engine
