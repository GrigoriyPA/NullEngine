#include "multithread_renderer.hpp"

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/acquire.hpp>
#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>

namespace null_engine {

using namespace detail;

namespace multithread {

using namespace detail;

namespace {

constexpr cl_int2 kClearBufferLocalSize = {.x = 256, .y = 1};

const std::string kClearBufferSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

    __kernel void clear_buffer(int2 image_size, __write_only image2d_t image, float3 background_color) {
        const int ix = get_global_id(0);
        const int iy = get_global_id(1);

        if (ix < image_size.x && iy < image_size.y) {
            write_imagef(image, (int2)(ix, iy), (float4)(background_color, 255.0f));
        }
    };
);

}  // anonymous namespace

Renderer::Renderer(const RendererSettings& settings, AccelerationContext context)
    : Base(settings, std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))
    , view_size_({.x = static_cast<cl_int>(view_width_), .y = static_cast<cl_int>(view_height_)})
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , clear_buffer_program_(compute::program::create_with_source(kClearBufferSource, context_))
    , buffer_(CreateBuffer())
    , rasterizer_(view_width_, view_height_, context) {
    BuildProgram(clear_buffer_program_);

    clear_buffer_kernel_ = compute::kernel(clear_buffer_program_, "clear_buffer");
    clear_buffer_kernel_.set_arg(0, view_size_);
    clear_buffer_kernel_.set_arg(1, buffer_.rasterizer_buffer.colors);
    clear_buffer_kernel_.set_arg(
        2, cl_float3{.x = background_color_.x(), .y = background_color_.y(), .z = background_color_.z()}
    );
}

void Renderer::SubscribeToTextures(InPort<GLuint>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, buffer_.rendering_texture);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    compute::opengl_enqueue_acquire_gl_objects(1, &buffer_.rasterizer_buffer.colors.get(), queue_);

    ClearBuffer();

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
        .depth = compute::buffer(context_, view_width_ * view_height_ * sizeof(FloatType))
    };

    return result;
}

void Renderer::ClearBuffer() {
    RunKernel(queue_, clear_buffer_kernel_, view_size_, kClearBufferLocalSize);
}

}  // namespace multithread

}  // namespace null_engine
