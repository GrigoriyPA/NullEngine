#include "multithread_rasterizer.hpp"

#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>

#include "common.hpp"

namespace null_engine {

using namespace detail;

namespace multithread::detail {

namespace {

constexpr cl_int2 kRasterizeLocalSize = {.x = 16, .y = 16};

const std::string kRasterizeTriangleSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
    struct VertexInfo {
        float4 pos;
        float3 color;
    };

    __kernel void TriangleRasterization(
        int2 image_size, __write_only image2d_t image, struct VertexInfo point_a, struct VertexInfo point_b,
        struct VertexInfo point_c
    ) {
        const int ix = get_global_id(0);
        const int iy = get_global_id(1);

        const float2 view_pos = (float2)((float)(ix) * 2.0f / (float)(image_size.x) - 1.0f,
                                         (float)(iy) * 2.0f / (float)(image_size.y) - 1.0f);

        const float denom = 1.0 / OrientedArea(point_a.pos.xy, point_b.pos.xy, point_c.pos.xy);
        const float3 barycentric = denom * (float3)(OrientedArea(view_pos, point_b.pos.xy, point_c.pos.xy),
                                                    OrientedArea(point_a.pos.xy, view_pos, point_c.pos.xy),
                                                    OrientedArea(point_a.pos.xy, point_b.pos.xy, view_pos));

        if (barycentric.x < -kEps || barycentric.y < -kEps || barycentric.z < -kEps) {
            return;
        }

        const float3 pos_w = (float3)(point_a.pos.w, point_b.pos.w, point_c.pos.w);
        const float3 perspective = barycentric * pos_w / dot(pos_w, barycentric);

        const float3 color =
            point_a.color * perspective.x + point_b.color * perspective.y + point_c.color * perspective.z;

        if (ix < image_size.x && iy < image_size.y) {
            write_imagef(image, (int2)(ix, iy), (float4)(color, 255.0));
        }
    };
);

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context)
    : view_size_({.x = static_cast<cl_int>(view_width), .y = static_cast<cl_int>(view_height)})
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , program_(compute::program::create_with_source({GetVectorFunctionsSource(), kRasterizeTriangleSource}, context_)) {
    BuildProgram(program_);

    kernel_ = compute::kernel(program_, "TriangleRasterization");
    kernel_.set_arg(0, view_size_);
}

void Rasterizer::DrawTriangles(
    const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
) {
    if (indices.empty()) {
        return;
    }

    kernel_.set_arg(1, buffer.colors);

    FillVerticesInfo(points);
    for (const auto [id_a, id_b, id_c] : indices) {
        kernel_.set_arg(2, sizeof(VertexInfo), &vertices_info_[id_a]);
        kernel_.set_arg(3, sizeof(VertexInfo), &vertices_info_[id_b]);
        kernel_.set_arg(4, sizeof(VertexInfo), &vertices_info_[id_c]);
        RunKernel(queue_, kernel_, view_size_, kRasterizeLocalSize);
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
        });
    }
}

}  // namespace multithread::detail

}  // namespace null_engine
