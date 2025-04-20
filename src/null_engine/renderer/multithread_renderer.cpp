#include "multithread_renderer.hpp"

#include <CL/cl_platform.h>

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/acquire.hpp>
#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/acceleration/program.hpp>

namespace null_engine {

using namespace detail;

namespace multithread {

using namespace detail;

Renderer::CleanupKernel::CleanupKernel(
    const RendererSettings& settings, const Buffer& buffer, AccelerationContext context
)
    : view_size_({.x = static_cast<cl_int>(settings.view_width), .y = static_cast<cl_int>(settings.view_height)})
    , kernel_("ClearBuffer", GetProgram(), context) {
    kernel_.MutableArgs()
        .SetVal(KA_VIEW_SIZE, view_size_)
        .SetVal(KA_VIEW, buffer.rasterizer_buffer.colors)
        .SetVal(KA_DEPTH, buffer.rasterizer_buffer.depth)
        .SetVal(KA_COLOR, Vec3ToCl(settings.background_color));
}

Program Renderer::CleanupKernel::GetProgram() {
    static constexpr std::string_view kClearBufferSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        __kernel void ClearBuffer(int2 view_size, __write_only image2d_t view, __global float* depth, float3 color) {
            const int2 i = (int2)(get_global_id(0), get_global_id(1));

            if (i.x < view_size.x && i.y < view_size.y) {
                write_imagef(view, i, (float4)(color, 1.0f));
                depth[i.x * view_size.y + i.y] = 1.0f;
            }
        }
    );

    return Program("RemdererClearBuffer", kClearBufferSource);
}

void Renderer::CleanupKernel::Run() {
    kernel_.Run(view_size_, kLocalSize);
}

Renderer::Renderer(const RendererSettings& settings, AccelerationContext context)
    : Base(settings, std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))
    , context_(context)
    , buffer_(CreateBuffer())
    , clear_buffer_kernel_(settings, buffer_, context)
    , fragment_shader_(context)
    , rasterizer_({.width = view_width_, .height = view_height_}, fragment_shader_, context) {
}

void Renderer::SubscribeToTextures(InPort<GLuint>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, buffer_.rendering_texture);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    auto& queue = context_.GetQueue();
    compute::opengl_enqueue_acquire_gl_objects(1, &buffer_.rasterizer_buffer.colors.get(), queue);
    clear_buffer_kernel_.Run();

    view_pos_ = render_event.camera->GetViewPos();
    camera_transform_ = render_event.camera->GetNdcTransform();
    rasterizer_.SetSceneInfo(fragment_shader_, view_pos_, render_event.scene.GetLights());

    for (const auto& [object, instances] : render_event.scene) {
        rasterizer_.SetMaterialInfo(fragment_shader_, object.GetMaterial());

        for (const auto& instance_transform : instances) {
            object_transform_ = instance_transform;

            if (object.IsTrianglesObject()) {
                RenderTrianglesObject(object);
            } else {
                assert(false && "Unsupported object type for rendering");
            }
        }
    }

    compute::opengl_enqueue_release_gl_objects(1, &buffer_.rasterizer_buffer.colors.get(), queue);
    queue.finish();

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

    auto& ctx = context_.GetContext();
    result.rasterizer_buffer = {
        .colors = compute::opengl_texture(ctx, GL_TEXTURE_2D, 0, result.rendering_texture, CL_MEM_WRITE_ONLY),
        .depth = compute::buffer(ctx, view_width_ * view_height_ * sizeof(FloatType))
    };

    return result;
}

}  // namespace multithread

}  // namespace null_engine
