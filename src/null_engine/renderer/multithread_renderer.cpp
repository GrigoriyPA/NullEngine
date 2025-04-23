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

Renderer::CleanupMainKernel::CleanupMainKernel(
    const RendererSettings& settings, const Buffer& buffer, AccelerationContext context
)
    : view_size_({.x = static_cast<cl_int>(settings.view_height), .y = static_cast<cl_int>(settings.view_width)})
    , kernel_("ClearBuffer", GetProgram(), context) {
    kernel_.MutableArgs()
        .SetVal(KA_VIEW_SIZE, view_size_)
        .SetVal(KA_VIEW, buffer.rasterizer_buffer.colors)
        .SetVal(KA_DEPTH, buffer.rasterizer_buffer.depth)
        .SetVal(KA_COLOR, Vec3ToCl(settings.background_color));
}

Program Renderer::CleanupMainKernel::GetProgram() {
    static constexpr std::string_view kClearBufferSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        __kernel void ClearBuffer(int2 view_size, __write_only image2d_t view, __global float* depth, float3 color) {
            const int2 i = (int2)(get_global_id(0), get_global_id(1));

            if (i.x < view_size.x && i.y < view_size.y) {
                write_imagef(view, i, (float4)(color, 1.0f));
                depth[i.x * view_size.y + i.y] = 1.0f;
            }
        }
    );

    return Program("RendererClearBuffer", kClearBufferSource);
}

void Renderer::CleanupMainKernel::Run() {
    kernel_.Run(view_size_, kLocalSize);
}

Renderer::CleanupDepthKernel::CleanupDepthKernel(AccelerationContext context)
    : kernel_("ClearBuffer", GetProgram(), context) {
}

Program Renderer::CleanupDepthKernel::GetProgram() {
    static constexpr std::string_view kClearBufferSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        __kernel void ClearBuffer(int size, __global float* depth) {
            const int i = get_global_id(0);

            if (i < size) {
                depth[i] = 1.0f;
            }
        }
    );

    return Program("RendererClearBuffer", kClearBufferSource);
}

void Renderer::CleanupDepthKernel::Run(cl_int size, const compute::buffer& buffer) {
    kernel_.MutableArgs().SetVal(KA_SIZE, size).SetVal(KA_DEPTH, buffer);
    kernel_.Run(size, kLocalSize);
}

Renderer::Renderer(const RendererSettings& settings, AccelerationContext context)
    : Base(settings, std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))
    , context_(context)
    , main_buffer_(CreateBuffer())
    , clear_main_buffer_kernel_(settings, main_buffer_, context)
    , clear_shadow_map_kernel_(context)
    , main_fragment_shader_(context)
    , main_rasterizer_({.width = view_width_, .height = view_height_}, main_fragment_shader_, context)
    , depth_rasterizer_({}, depth_fragment_shader_, context)
    , shadows_map_(context_) {
}

void Renderer::SubscribeToTextures(InPort<GLuint>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, main_buffer_.rendering_texture);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    auto& queue = context_.GetQueue();
    compute::opengl_enqueue_acquire_gl_objects(1, &main_buffer_.rasterizer_buffer.colors.get(), queue);

    RenderScene(render_event.scene, render_event.camera);

    compute::opengl_enqueue_release_gl_objects(1, &main_buffer_.rasterizer_buffer.colors.get(), queue);
    queue.finish();

    out_texture_port_->Notify(main_buffer_.rendering_texture);
}

void Renderer::FillShadowsMap(const Scene& scene) {
    const auto& lights = scene.GetLights();
    assert(lights.size() <= MainFragmentShader::kMaxNumberLights && "Too many lights provided");

    cl_int buffer_size = 0;
    for (size_t i = 0; i < lights.size(); ++i) {
        shadows_info_.buffer_offsets[i] = buffer_size;
        if (const auto& shadow_info = lights[i].GetShadowInfo()) {
            buffer_size += shadow_info->shadow_width * shadow_info->shadow_height;
        }
    }
    shadows_map_.Reserve(buffer_size);
    shadows_info_.depth_buffer = shadows_map_.GetBuffer();
    clear_shadow_map_kernel_.Run(buffer_size, shadows_map_.GetBuffer());

    for (size_t i = 0; i < lights.size(); ++i) {
        const auto& shadow = lights[i].GetShadowInfo();
        if (!shadow) {
            continue;
        }

        depth_rasterizer_.UpdateView({.width = shadow->shadow_width, .height = shadow->shadow_height});
        RasterizerBuffer buffer = {.depth = shadows_map_.GetBuffer()};
        RenderingContext context = {
            .rasterizer = depth_rasterizer_,
            .buffer = buffer,
            .view_pos = shadow->light_pos,
            .camera_transform = shadow->transform
        };
    }
}

void Renderer::RenderScene(const Scene& scene, AnyCameraRef camera) {
    clear_main_buffer_kernel_.Run();

    const auto view_pos = camera->GetViewPos();
    RenderingContext context = {
        .rasterizer = main_rasterizer_,
        .buffer = main_buffer_.rasterizer_buffer,
        .view_pos = view_pos,
        .camera_transform = camera->GetNdcTransform()
    };

    main_fragment_shader_.FillSceneInfo(main_rasterizer_.GetShaderArgs(), view_pos, scene.GetLights(), shadows_info_);
    for (const auto& [object, instances] : scene) {
        main_fragment_shader_.FillMaterialInfo(main_rasterizer_.GetShaderArgs(), object.GetMaterial());

        RenderObject(object, instances, context);
    }
}

void Renderer::RenderObject(
    const VerticesObject& object, const std::vector<Transform>& instances, RenderingContext& context
) {
    for (const auto& instance_transform : instances) {
        context.object_transform = instance_transform;

        if (object.IsTrianglesObject()) {
            RenderTrianglesObject(object, context);
        } else {
            assert(false && "Unsupported object type for rendering");
        }
    }
}

void Renderer::RenderTrianglesObject(const VerticesObject& object, RenderingContext& context) {
    assert(object.IsTrianglesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipTriangles(
        context.view_pos,
        ConvertObjectVerices(context.camera_transform, context.object_transform, object.GetVertices()),
        object.GetTriangleIndices()
    );

    context.rasterizer.DrawTriangles(clipped.vertices, clipped.indices, context.buffer);
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
