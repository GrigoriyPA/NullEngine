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

        typedef struct {
            int3 triangles[TRAINGLES_IN_BATCH];
            int number_traingles;
        } WorkBatch;

        float3 weighted_sumf3(float3 a, float3 b, float3 c, float3 perspective) {
            return a * perspective.x + b * perspective.y + c * perspective.z;
        }

        float2 weighted_sumf2(float2 a, float2 b, float2 c, float3 perspective) {
            return a * perspective.x + b * perspective.y + c * perspective.z;
        }

        __kernel void TriangleRasterization(
            int2 view_size, __write_only image2d_t view, __global float* depth, int2 work_size,
            __global WorkBatch* works, __global VertexInfo* points,
            // clang-format off
            <|FRAGMENT_SHADER_ARGS|>
            // clang-format on
        ) {
            const int2 gi = (int2)(get_group_id(0), get_group_id(1));
            if (gi.x >= work_size.x || gi.y >= work_size.y) {
                return;
            }
            const WorkBatch* work = &works[gi.x * work_size.y + gi.y];

            const int2 i = (int2)(get_global_id(0), get_global_id(1));
            if (i.x >= view_size.x || i.y >= view_size.y) {
                return;
            }

            const float2 view_pos = (float2)((float)(i.x) * 2.0f / (float)(view_size.x) - 1.0f,
                                             (float)(i.y) * 2.0f / (float)(view_size.y) - 1.0f);

            for (int id = 0; id < TRAINGLES_IN_BATCH; ++id) {
                if (id >= work->number_traingles) {
                    break;
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
        .Build();
}

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context)
    : view_size_({.x = static_cast<cl_int>(view_width), .y = static_cast<cl_int>(view_height)})
    , work_size_({.x = view_size_.x / kRasterizeKernelLocalSize.x, .y = view_size_.y / kRasterizeKernelLocalSize.y})
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , program_(GetRasterizerKernelProgram())
    , kernel_(program_.BuildKernel("TriangleRasterization", context))
    , work_batches_buffer_(context_, work_size_.x * work_size_.y * sizeof(WorkBatch))
    , vertices_info_buffer_(context_, 0) {
    kernel_.set_arg(KA_VIEW_SIZE, view_size_);
    kernel_.set_arg(KA_WORK_SIZE, work_size_);
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
    FillWorkBatches(indices);

    kernel_.set_arg(KA_POINTS, vertices_info_buffer_);

    for (size_t i = 0; i < number_works_; ++i) {
        queue_.enqueue_write_buffer(
            work_batches_buffer_, 0, work_batches_[i].size() * sizeof(WorkBatch), work_batches_[i].data()
        );
        kernel_.set_arg(KA_WORK, work_batches_buffer_);

        RunKernel(queue_, kernel_, view_size_, kRasterizeKernelLocalSize);
    }
}

void Rasterizer::FillVerticesInfo(const std::vector<InterpVertex>& points) {
    vertices_info_.clear();
    vertex_pos_.clear();
    vertices_info_.reserve(points.size());
    vertex_pos_.reserve(points.size());
    for (auto [position, params] : points) {
        PerspectiveDivision(position);

        vertices_info_.push_back({
            .pos = Vec4ToCl(position),
            .color = Vec3ToCl(params.color),
            .normal = Vec3ToCl(params.normal),
            .tex_coords = Vec2ToCl(params.tex_coords),
            .frag_pos = Vec3ToCl(params.frag_pos),
        });

        vertex_pos_.push_back({
            .x = std::max(
                0,
                std::min(static_cast<cl_int>(std::floor(work_size_.x * (position.x() + 1.0) / 2.0)), work_size_.x - 1)
            ),
            .y = std::max(
                0,
                std::min(static_cast<cl_int>(std::floor(work_size_.y * (position.y() + 1.0) / 2.0)), work_size_.y - 1)
            ),
        });
    }

    if (vertices_info_buffer_.size() < vertices_info_.size() * sizeof(VertexInfo)) {
        vertices_info_buffer_ = compute::buffer(context_, vertices_info_.size() * sizeof(VertexInfo));
    }
    queue_.enqueue_write_buffer(
        vertices_info_buffer_, 0, vertices_info_.size() * sizeof(VertexInfo), vertices_info_.data()
    );
}

void Rasterizer::FillWorkBatches(const std::vector<TriangleIndex>& indices) {
    number_works_ = 0;
    batch_id_.assign(work_size_.x * work_size_.y, 0);
    for (const auto [id_a, id_b, id_c] : indices) {
        auto a = vertex_pos_[id_a];
        auto b = vertex_pos_[id_b];
        auto c = vertex_pos_[id_c];

        for (cl_int i = std::min(a.x, std::min(b.x, c.x)); i <= std::max(a.x, std::max(b.x, c.x)); ++i) {
            for (cl_int j = std::min(a.y, std::min(b.y, c.y)); j <= std::max(a.y, std::max(b.y, c.y)); ++j) {
                const auto p = i * work_size_.y + j;
                if (batch_id_[p] >= number_works_) {
                    number_works_++;
                    if (number_works_ >= work_batches_.size()) {
                        work_batches_.emplace_back(work_size_.x * work_size_.y, WorkBatch{.number_traingles = 0});
                    } else {
                        for (auto& r : work_batches_[batch_id_[p]]) {
                            r.number_traingles = 0;
                        }
                    }
                }
                auto* batch = &work_batches_[batch_id_[p]][i * work_size_.y + j];
                assert(batch->number_traingles < kTrianglesInBatch);
                batch->triangles[batch->number_traingles++] = cl_int3{
                    .x = static_cast<cl_int>(id_a),
                    .y = static_cast<cl_int>(id_b),
                    .z = static_cast<cl_int>(id_c),
                };
                if (batch->number_traingles >= kTrianglesInBatch) {
                    batch_id_[p]++;
                }
            }
        }
    }
}

}  // namespace multithread::detail

}  // namespace null_engine
