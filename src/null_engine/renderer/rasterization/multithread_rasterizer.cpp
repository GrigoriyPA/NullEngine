#include "multithread_rasterizer.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <fmt/core.h>

#include <boost/compute/kernel.hpp>
#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/renderer/shaders/multithread_fragment_shader.hpp>
#include <null_engine/util/generic/validation.hpp>
#include <string>

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
    KA_WORK_SIZE,
    KA_WORK,
    KA_POINTS,
    KA_NUMBER_TRIANGLES,
    KA_WORK_OFFSET,
    KA_SHADER_PARAMS,
};

Program GetRasterizerDefenitions() {
    static constexpr std::string_view kRasterizerDefenitionsSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        typedef struct {
            float4 pos;
            float3 color;
            float3 normal;
            float2 tex_coords;
            float3 frag_pos;
        } VertexInfo;

        typedef struct { int3 triangles[TRAINGLES_IN_BATCH]; } WorkBatch;
    );

    return ProgramBuilder("RasterizerDefenitions", kRasterizerDefenitionsSource)
        .Define("TRAINGLES_IN_BATCH", std::to_string(Rasterizer::kTrianglesInBatch))
        .Build();
}

Program GetRasterizerKernelProgram() {
    static constexpr std::string_view kRasterizerSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        float3 weighted_sumf3(float3 a, float3 b, float3 c, float3 perspective) {
            return a * perspective.x + b * perspective.y + c * perspective.z;
        }

        float2 weighted_sumf2(float2 a, float2 b, float2 c, float3 perspective) {
            return a * perspective.x + b * perspective.y + c * perspective.z;
        }

        __kernel void TriangleRasterization(
            int2 view_size, __write_only image2d_t view, __global float* depth, int2 work_size,
            __global WorkBatch* works, __global VertexInfo* points, __global int* numbers_triangles, int work_offset,
            // clang-format off
            <|FRAGMENT_SHADER_ARGS|>
            // clang-format on
        ) {
            const int2 gi = (int2)(get_group_id(0), get_group_id(1));
            if (gi.x >= work_size.x || gi.y >= work_size.y) {
                return;
            }
            const int number_triangles =
                numbers_triangles[gi.x * work_size.y + gi.y] - work_offset * TRAINGLES_IN_BATCH;
            const WorkBatch* work = &works[work_offset * work_size.x * work_size.y + gi.x * work_size.y + gi.y];

            const int2 i = (int2)(get_global_id(0), get_global_id(1));
            if (i.x >= view_size.x || i.y >= view_size.y) {
                return;
            }

            const float2 view_pos = (float2)((float)(i.x) * 2.0f / (float)(view_size.x) - 1.0f,
                                             (float)(i.y) * 2.0f / (float)(view_size.y) - 1.0f);

            for (int id = 0; id < TRAINGLES_IN_BATCH; ++id) {
                if (id >= number_triangles) {
                    return;
                }

                const int3 triangle = work->triangles[id];
                const VertexInfo point_a = points[triangle.x];
                const VertexInfo point_b = points[triangle.y];
                const VertexInfo point_c = points[triangle.z];

                const float denom = 1.0 / OrientedArea(point_a.pos.xy, point_b.pos.xy, point_c.pos.xy);
                const float3 barycentric = denom * (float3)(OrientedArea(view_pos, point_b.pos.xy, point_c.pos.xy),
                                                            OrientedArea(point_a.pos.xy, view_pos, point_c.pos.xy),
                                                            OrientedArea(point_a.pos.xy, point_b.pos.xy, view_pos));

                if (barycentric.x < -kEps || barycentric.y < -kEps || barycentric.z < -kEps) {
                    continue;
                }

                const float z = dot((float3)(point_a.pos.z, point_b.pos.z, point_c.pos.z), barycentric);
                if (z <= -1.0f || depth[i.x * view_size.y + i.y] <= z) {
                    continue;
                }

                const float3 pos_w = (float3)(point_a.pos.w, point_b.pos.w, point_c.pos.w);
                const float3 perspective = barycentric * pos_w / dot(pos_w, barycentric);
                const InterpolationParams params = {
                    .color = weighted_sumf3(point_a.color, point_b.color, point_c.color, perspective),
                    .normal = weighted_sumf3(point_a.normal, point_b.normal, point_c.normal, perspective),
                    .tex_coords =
                        weighted_sumf2(point_a.tex_coords, point_b.tex_coords, point_c.tex_coords, perspective),
                    .frag_pos = weighted_sumf3(point_a.frag_pos, point_b.frag_pos, point_c.frag_pos, perspective),
                };

                // clang-format off
                const float3 color = <|FRAGMENT_SHADER_CALL|>;
                // clang-format on

                write_imagef(view, (int2)(i.x, i.y), (float4)(color, 1.0f));
                depth[i.x * view_size.y + i.y] = z;
            }
        }
    );

    return ProgramBuilder("Rasterizer", kRasterizerSource)
        .Define("TRAINGLES_IN_BATCH", std::to_string(Rasterizer::kTrianglesInBatch))
        .Replace("FRAGMENT_SHADER_ARGS", FragmentShader::GetArguments())
        .Replace("FRAGMENT_SHADER_CALL", FragmentShader::GetShaderCall("params"))
        .Include(GetVectorFunctionsProgram())
        .Include(FragmentShader::GetKernelProgram())
        .Include(GetRasterizerDefenitions())
        .Build();
}

constexpr cl_int kDistributeKernelLocalSize = 256;

enum DArgs {
    KA_I_SIZE,
    KA_I,
    KA_W_SIZE,
    KA_W,
    KA_P,
    KA_NRT,
};

Program GetRasterizerDistributionProgram() {
    static constexpr std::string_view kRasterizerDistributionSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        int2 get_coord(int2 work_size, const VertexInfo* point) {
            const float4 position = point->pos;
            const int x = fmax(0.0f, fmin(floor((float)work_size.x * (position.x + 1.0f) / 2.0f), work_size.x - 1));
            const int y = fmax(0.0f, fmin(floor((float)work_size.y * (position.y + 1.0f) / 2.0f), work_size.y - 1));
            return (int2)(x, y);
        }

        __kernel void RasterizerDistribution(
            int indices_size, __global int3* indices, int2 work_size, __global WorkBatch* works,
            __global VertexInfo* points, __global int* numbers_triangles
        ) {
            const int id = get_global_id(0);
            if (id >= indices_size) {
                return;
            }

            int3 index = indices[id];
            int2 a = get_coord(work_size, &points[index.x]);
            int2 b = get_coord(work_size, &points[index.y]);
            int2 c = get_coord(work_size, &points[index.z]);

            for (int i = min(a.x, min(b.x, c.x)); i <= max(a.x, max(b.x, c.x)); ++i) {
                for (int j = min(a.y, min(b.y, c.y)); j <= max(a.y, max(b.y, c.y)); ++j) {
                    const int p = i * work_size.y + j;
                    const int nr_triangles = atomic_add(&numbers_triangles[p], 1);
                    if (nr_triangles >= NUMBER_WORKS * TRAINGLES_IN_BATCH) {
                        return;
                    }
                    WorkBatch* batch = &works[(nr_triangles / TRAINGLES_IN_BATCH) * work_size.x * work_size.y + p];
                    batch->triangles[nr_triangles % TRAINGLES_IN_BATCH] = index;
                }
            }
        }
    );

    return ProgramBuilder("RasterizerDistribution", kRasterizerDistributionSource)
        .Define("TRAINGLES_IN_BATCH", std::to_string(Rasterizer::kTrianglesInBatch))
        .Define("NUMBER_WORKS", std::to_string(Rasterizer::kNumberWorks))
        .Include(GetRasterizerDefenitions())
        .Build();
}

Program GetRasterizerCleanupProgram() {
    static constexpr std::string_view kRasterizerCleanupSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        __kernel void RasterizerCleanup(int2 work_size, __global int* numbers_triangles) {
            const int2 i = (int2)(get_global_id(0), get_global_id(1));
            if (i.x >= work_size.x || i.y >= work_size.y) {
                return;
            }

            numbers_triangles[i.x * work_size.y + i.y] = 0;
        }
    );

    return Program("RasterizerCleanup", kRasterizerCleanupSource);
}

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context)
    : view_size_({.x = static_cast<cl_int>(view_width), .y = static_cast<cl_int>(view_height)})
    , work_size_({.x = view_size_.x / kRasterizeKernelLocalSize.x, .y = view_size_.y / kRasterizeKernelLocalSize.y})
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , program_(GetRasterizerKernelProgram())
    , kernel_(program_.BuildKernel("TriangleRasterization", context))
    , vertices_info_buffer_(context_, 0)
    , distribution_program_(GetRasterizerDistributionProgram())
    , distribution_kernel_(distribution_program_.BuildKernel("RasterizerDistribution", context))
    , number_triangles_buffer_(context_, work_size_.x * work_size_.y * sizeof(cl_int))
    , work_batches_buffer_(context_, kNumberWorks * work_size_.x * work_size_.y * sizeof(WorkBatch))
    , indices_buffer_(context_, 0)
    , cleanup_program_(GetRasterizerCleanupProgram())
    , cleanup_kernel_(cleanup_program_.BuildKernel("RasterizerCleanup", context)) {
    kernel_.set_arg(KA_VIEW_SIZE, view_size_);
    kernel_.set_arg(KA_WORK_SIZE, work_size_);
    kernel_.set_arg(KA_WORK, work_batches_buffer_);
    kernel_.set_arg(KA_NUMBER_TRIANGLES, number_triangles_buffer_);

    distribution_kernel_.set_arg(KA_W_SIZE, work_size_);
    distribution_kernel_.set_arg(KA_W, work_batches_buffer_);
    distribution_kernel_.set_arg(KA_NRT, number_triangles_buffer_);

    cleanup_kernel_.set_arg(0, work_size_);
    cleanup_kernel_.set_arg(1, number_triangles_buffer_);
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
    RunKernel(queue_, cleanup_kernel_, work_size_, kRasterizeKernelLocalSize);

    FillVerticesInfo(points);

    indices_.clear();
    indices_.reserve(indices.size());
    for (const auto [id_a, id_b, id_c] : indices) {
        indices_.push_back({
            .x = static_cast<cl_int>(id_a),
            .y = static_cast<cl_int>(id_b),
            .z = static_cast<cl_int>(id_c),
        });
    }
    if (indices_buffer_.size() < indices_.size() * sizeof(cl_int3)) {
        indices_buffer_ = compute::buffer(context_, indices_.size() * sizeof(cl_int3));
    }
    queue_.enqueue_write_buffer(indices_buffer_, 0, indices_.size() * sizeof(cl_int3), indices_.data());
    distribution_kernel_.set_arg(KA_I_SIZE, static_cast<cl_int>(indices.size()));
    distribution_kernel_.set_arg(KA_I, indices_buffer_);
    distribution_kernel_.set_arg(KA_P, vertices_info_buffer_);
    RunKernel(queue_, distribution_kernel_, indices.size(), kDistributeKernelLocalSize);

    kernel_.set_arg(KA_VIEW, buffer.colors);
    kernel_.set_arg(KA_DEPTH, buffer.depth);
    kernel_.set_arg(KA_POINTS, vertices_info_buffer_);
    for (size_t i = 0; i < kNumberWorks; ++i) {
        kernel_.set_arg(KA_WORK_OFFSET, static_cast<cl_int>(i));
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

    if (vertices_info_buffer_.size() < vertices_info_.size() * sizeof(VertexInfo)) {
        vertices_info_buffer_ = compute::buffer(context_, vertices_info_.size() * sizeof(VertexInfo));
    }
    queue_.enqueue_write_buffer(
        vertices_info_buffer_, 0, vertices_info_.size() * sizeof(VertexInfo), vertices_info_.data()
    );
}

}  // namespace multithread::detail

}  // namespace null_engine
