#include "multithread_rasterizer.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <fmt/core.h>

#include <boost/compute/kernel.hpp>
#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/renderer/shaders/multithread_fragment_shader.hpp>

#include "common.hpp"

namespace null_engine {

using namespace detail;

namespace multithread::detail {

namespace {

constexpr cl_int2 kRasterizeKernelLocalSize = {.x = 16, .y = 16};

enum KernelArgs {
    KA_VIEW_SIZE,
    KA_VIEW,
    KA_DEPTH,
    KA_POINT_A,
    KA_POINT_B,
    KA_POINT_C,
    KA_SHADER_PARAMS,
};

Program GetRasterizerKernelProgram() {
    static constexpr std::string_view kRasterizerSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        typedef struct {
            float4 pos;
            float3 color;
            float3 normal;
            float2 tex_coords;
            float3 frag_pos;
        } VertexInfo;

        float3 weighted_sumf3(float3 a, float3 b, float3 c, float3 perspective) {
            return a * perspective.x + b * perspective.y + c * perspective.z;
        }

        float2 weighted_sumf2(float2 a, float2 b, float2 c, float3 perspective) {
            return a * perspective.x + b * perspective.y + c * perspective.z;
        }

        __kernel void TriangleRasterization(
            int2 view_size, __write_only image2d_t view, __global float* depth, VertexInfo point_a, VertexInfo point_b,
            VertexInfo point_c,
            // clang-format off
            <|FRAGMENT_SHADER_ARGS|>
            // clang-format on
        ) {
            const int2 i = (int2)(get_global_id(0), get_global_id(1));
            if (i.x >= view_size.x || i.y >= view_size.y) {
                return;
            }

            const float2 view_pos = (float2)((float)(i.x) * 2.0f / (float)(view_size.x) - 1.0f,
                                             (float)(i.y) * 2.0f / (float)(view_size.y) - 1.0f);

            const float denom = 1.0 / OrientedArea(point_a.pos.xy, point_b.pos.xy, point_c.pos.xy);
            const float3 barycentric = denom * (float3)(OrientedArea(view_pos, point_b.pos.xy, point_c.pos.xy),
                                                        OrientedArea(point_a.pos.xy, view_pos, point_c.pos.xy),
                                                        OrientedArea(point_a.pos.xy, point_b.pos.xy, view_pos));

            if (barycentric.x < -kEps || barycentric.y < -kEps || barycentric.z < -kEps) {
                return;
            }

            const float3 pos_w = (float3)(point_a.pos.w, point_b.pos.w, point_c.pos.w);
            const float3 perspective = barycentric * pos_w / dot(pos_w, barycentric);

            const float3 pos_z = (float3)(point_a.pos.z, point_b.pos.z, point_c.pos.z);
            const float z = dot(pos_z, perspective);
            if (z <= -1.0f || depth[i.x * view_size.y + i.y] <= z) {
                return;
            }

            const InterpolationParams params = {
                .color = weighted_sumf3(point_a.color, point_b.color, point_c.color, perspective),
                .normal = weighted_sumf3(point_a.normal, point_b.normal, point_c.normal, perspective),
                .tex_coords = weighted_sumf2(point_a.tex_coords, point_b.tex_coords, point_c.tex_coords, perspective),
                .frag_pos = weighted_sumf3(point_a.frag_pos, point_b.frag_pos, point_c.frag_pos, perspective),
            };

            // clang-format off
            const float3 color = <|FRAGMENT_SHADER_CALL|>;
            // clang-format on

            write_imagef(view, (int2)(i.x, i.y), (float4)(color, 1.0f));
            depth[i.x * view_size.y + i.y] = z;
        }
    );

    return ProgramBuilder("Rasterizer", kRasterizerSource)
        .Replace("FRAGMENT_SHADER_ARGS", FragmentShader::GetArguments())
        .Replace("FRAGMENT_SHADER_CALL", FragmentShader::GetShaderCall("params"))
        .Include(GetVectorFunctionsProgram())
        .Include(FragmentShader::GetKernelProgram())
        .Build();
}

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context)
    : view_size_({.x = static_cast<cl_int>(view_width), .y = static_cast<cl_int>(view_height)})
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , program_(GetRasterizerKernelProgram())
    , kernel_(program_.BuildKernel("TriangleRasterization", context)) {
    kernel_.set_arg(KA_VIEW_SIZE, view_size_);
}

void Rasterizer::SetSceneInfo(const FragmentShader& shader, Vec3 view_pos, const std::vector<AnyLight>& lights) {
    shader.FillSceneInfo(kernel_, KA_SHADER_PARAMS, view_pos, lights);
}

void Rasterizer::SetMaterialInfo(const FragmentShader& shader, const Material& material) {
    shader.FillMaterialInfo(kernel_, KA_SHADER_PARAMS, material);
}

void Rasterizer::DrawTriangles(
    const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
) {
    if (indices.empty()) {
        return;
    }

    kernel_.set_arg(KA_VIEW, buffer.colors);
    kernel_.set_arg(KA_DEPTH, buffer.depth);

    FillVerticesInfo(points);
    for (const auto [id_a, id_b, id_c] : indices) {
        kernel_.set_arg(KA_POINT_A, sizeof(VertexInfo), &vertices_info_[id_a]);
        kernel_.set_arg(KA_POINT_B, sizeof(VertexInfo), &vertices_info_[id_b]);
        kernel_.set_arg(KA_POINT_C, sizeof(VertexInfo), &vertices_info_[id_c]);
        RunKernel(queue_, kernel_, view_size_, kRasterizeKernelLocalSize);
    }
}

void Rasterizer::FillVerticesInfo(const std::vector<InterpVertex>& points) {
    vertices_info_.clear();
    vertices_info_.reserve(points.size());
    for (auto [position, params] : points) {
        PerspectiveDivision(position);

        vertices_info_.push_back({
            .pos = Vec4ToCl(position),
            .color = Vec3ToCl(params.color),
            .normal = Vec3ToCl(params.normal),
            .tex_coords = Vec2ToCl(params.tex_coords),
            .frag_pos = Vec3ToCl(params.frag_pos),
        });
    }
}

}  // namespace multithread::detail

}  // namespace null_engine
