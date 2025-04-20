#include "multithread_rasterizer.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <fmt/core.h>

#include <boost/compute/buffer.hpp>
#include <boost/compute/kernel.hpp>
#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/acceleration/kernel.hpp>
#include <null_engine/renderer/shaders/multithread_fragment_shader.hpp>
#include <null_engine/util/generic/validation.hpp>
#include <string>

#include "common.hpp"

namespace null_engine {

using namespace detail;

namespace multithread::detail {

Rasterizer::RasterizationKernel::RasterizationKernel(
    ViewInfo view, const SharedBuffers& buffers, const FragmentShader& fragment_shader, AccelerationContext context
)
    : view_size_({.x = static_cast<cl_int>(view.width), .y = static_cast<cl_int>(view.height)})
    , kernel_("TriangleRasterization", GetProgram(fragment_shader), context) {
    kernel_.MutableArgs()
        .SetVal(KA_VIEW_SIZE, view_size_)
        .SetVal(KA_WORK_SIZE, GetWorkSize(view))
        .SetVal(KA_WORK, buffers.work_batches)
        .SetVal(KA_NUMBER_TRIANGLES, buffers.number_triangles);
}

Program Rasterizer::RasterizationKernel::GetProgram(const FragmentShader& fragment_shader) {
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
                const float3 color = CalculateFragmentColor(<|FRAGMENT_SHADER_CALL|>, &params);
                // clang-format on

                write_imagef(view, (int2)(i.x, i.y), (float4)(color, 1.0f));
                depth[i.x * view_size.y + i.y] = z;
            }
        }
    );

    const auto& shader_args = fragment_shader.GetArgs();
    return ProgramBuilder("Rasterizer", kRasterizerSource)
        .Define("TRAINGLES_IN_BATCH", kTrianglesInBatch)
        .Replace("FRAGMENT_SHADER_ARGS", shader_args.GetArgsDefenition())
        .Replace("FRAGMENT_SHADER_CALL", shader_args.GetArgsForward())
        .Include(GetVectorFunctionsProgram())
        .Include(fragment_shader.GetProgram())
        .Include(GetRasterizerDefenitions())
        .Build();
}

Kernel::Args Rasterizer::RasterizationKernel::GetShaderArgs() {
    return kernel_.MutableArgs(KA_SHADER_PARAMS);
}

void Rasterizer::RasterizationKernel::Run(const compute::buffer& vertices_info_buffer, const RasterizerBuffer& buffer) {
    kernel_.MutableArgs()
        .SetVal(KA_VIEW, buffer.colors)
        .SetVal(KA_DEPTH, buffer.depth)
        .SetVal(KA_POINTS, vertices_info_buffer);

    for (size_t i = 0; i < kNumberWorks; ++i) {
        kernel_.MutableArgs().SetVal(KA_WORK_OFFSET, static_cast<cl_int>(i));
        kernel_.Run(view_size_, kWorkShape);
    }
}

Rasterizer::DistributionKernel::DistributionKernel(
    ViewInfo view, const SharedBuffers& buffers, AccelerationContext context
)
    : indices_(context)
    , kernel_("RasterizerDistribution", GetProgram(), context) {
    kernel_.MutableArgs()
        .SetVal(KA_WORK_SIZE, GetWorkSize(view))
        .SetVal(KA_WORK, buffers.work_batches)
        .SetVal(KA_NUMBER_TRIANGLES, buffers.number_triangles);
}

Program Rasterizer::DistributionKernel::GetProgram() {
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
        .Define("TRAINGLES_IN_BATCH", kTrianglesInBatch)
        .Define("NUMBER_WORKS", kNumberWorks)
        .Include(GetRasterizerDefenitions())
        .Build();
}

void Rasterizer::DistributionKernel::Run(
    const compute::buffer& vertices_info_buffer, const std::vector<TriangleIndex>& indices
) {
    indices_.Assign<TriangleIndex>(indices, [](const TriangleIndex& index) {
        return cl_int3{
            .x = static_cast<cl_int>(index.point_a),
            .y = static_cast<cl_int>(index.point_b),
            .z = static_cast<cl_int>(index.point_c),
        };
    });

    kernel_.MutableArgs()
        .SetVal(KA_INDICES_SIZE, static_cast<cl_int>(indices.size()))
        .SetVal(KA_INDICES, indices_.GetBuffer())
        .SetVal(KA_POINTS, vertices_info_buffer);

    kernel_.Run(static_cast<cl_int>(indices.size()), kLocalSize);
}

Rasterizer::CleanupKernel::CleanupKernel(ViewInfo view, const SharedBuffers& buffers, AccelerationContext context)
    : work_size_(GetWorkSize(view))
    , kernel_("RasterizerCleanup", GetProgram(), context) {
    kernel_.MutableArgs().SetVal(KA_WORK_SIZE, work_size_).SetVal(KA_NUMBER_TRIANGLES, buffers.number_triangles);
}

Program Rasterizer::CleanupKernel::GetProgram() {
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

void Rasterizer::CleanupKernel::Run() {
    kernel_.Run(work_size_, kLocalSize);
}

Rasterizer::Rasterizer(ViewInfo view, const FragmentShader& fragment_shader, AccelerationContext context)
    : context_(context)
    , shared_buffers_(CreateBuffers(view))
    , vertices_info_(context_)
    , rasterization_kernel_(view, shared_buffers_, fragment_shader, context)
    , distribution_kernel_(view, shared_buffers_, context)
    , cleanup_kernel_(view, shared_buffers_, context) {
}

void Rasterizer::SetSceneInfo(const FragmentShader& shader, Vec3 view_pos, const std::vector<AnyLight>& lights) {
    shader.FillSceneInfo(rasterization_kernel_.GetShaderArgs(), view_pos, lights);
}

void Rasterizer::SetMaterialInfo(const FragmentShader& shader, const Material& material) {
    shader.FillMaterialInfo(rasterization_kernel_.GetShaderArgs(), material);
}

void Rasterizer::DrawTriangles(
    const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
) {
    if (indices.empty()) {
        return;
    }

    FillVerticesInfoBuffer(points);

    cleanup_kernel_.Run();
    distribution_kernel_.Run(vertices_info_.GetBuffer(), indices);
    rasterization_kernel_.Run(vertices_info_.GetBuffer(), buffer);
}

void Rasterizer::FillVerticesInfoBuffer(const std::vector<InterpVertex>& points) {
    vertices_info_.Assign<InterpVertex>(points, [](const InterpVertex& vertex) {
        auto position = vertex.position;
        PerspectiveDivision(position);

        return VertexInfo{
            .pos = Vec4ToCl(position),
            .color = Vec3ToCl(vertex.params.color),
            .normal = Vec3ToCl(vertex.params.normal),
            .tex_coords = Vec2ToCl(vertex.params.tex_coords),
            .frag_pos = Vec3ToCl(vertex.params.frag_pos),
        };
    });
}

Rasterizer::SharedBuffers Rasterizer::CreateBuffers(const ViewInfo& view) {
    const auto work_size = GetWorkSize(view);
    const auto buffer_size = work_size.x * work_size.y;

    auto& ctx = context_.GetContext();
    return {
        .work_batches = compute::buffer(ctx, kNumberWorks * buffer_size * sizeof(WorkBatch)),
        .number_triangles = compute::buffer(ctx, buffer_size * sizeof(cl_int)),
    };
}

cl_int2 Rasterizer::GetWorkSize(const ViewInfo& view) {
    return {.x = static_cast<cl_int>(view.width / kWorkShape.x), .y = static_cast<cl_int>(view.height / kWorkShape.y)};
}

Program Rasterizer::GetRasterizerDefenitions() {
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
        .Define("TRAINGLES_IN_BATCH", kTrianglesInBatch)
        .Build();
}

}  // namespace multithread::detail

}  // namespace null_engine
