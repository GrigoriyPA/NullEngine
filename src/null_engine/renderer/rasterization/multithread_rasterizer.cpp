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
    struct VertexInfo { float2 pos; };

    float OrientedArea(float2 point_a, float2 point_b, float2 point_c) {
        const float2 left = point_b - point_a;
        const float2 right = point_c - point_a;
        return left.y * right.x - left.x * right.y;
    }

    __kernel void TriangleRasterization(
        int2 image_size, __write_only image2d_t image, struct VertexInfo point_a, struct VertexInfo point_b,
        struct VertexInfo point_c
    ) {
        const int ix = get_global_id(0);
        const int iy = get_global_id(1);

        const float2 view_pos = (float2)((float)(ix) * 2.0f / (float)(image_size.x) - 1.0f,
                                         (float)(iy) * 2.0f / (float)(image_size.y) - 1.0f);

        const float denom = 1.0 / OrientedArea(point_a.pos, point_b.pos, point_c.pos);
        const float3 barycentric = denom * (float3)(OrientedArea(view_pos, point_b.pos, point_c.pos),
                                                    OrientedArea(point_a.pos, view_pos, point_c.pos),
                                                    OrientedArea(point_a.pos, point_b.pos, view_pos));

        if (barycentric.x <= 0.0 || barycentric.y <= 0.0 || barycentric.z <= 0.0) {
            return;
        }

        if (ix < image_size.x && iy < image_size.y) {
            write_imagef(image, (int2)(ix, iy), (float4)(0.0f, 200.0f, 0.0f, 255.0));
        }
    };
);

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context)
    : view_size_({.x = static_cast<cl_int>(view_width), .y = static_cast<cl_int>(view_height)})
    , context_(context.GetContext())
    , queue_(context.GetQueue())
    , program_(compute::program::create_with_source(kRasterizeTriangleSource, context_)) {
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

        const cl_float2 pos = {
            .x = static_cast<cl_float>(position.x()),
            .y = static_cast<cl_float>(position.y()),
        };

        vertices_info_.push_back({.pos = pos});
    }
}

}  // namespace multithread::detail

}  // namespace null_engine
