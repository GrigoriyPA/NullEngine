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
    struct VertexInfo { int2 pos; };

    __kernel void triangle_rasterization(
        int2 image_size, __write_only image2d_t image, struct VertexInfo point_a, struct VertexInfo point_b,
        struct VertexInfo point_c
    ) {
        const int ix = get_global_id(0);
        const int iy = get_global_id(1);

        if (ix < image_size.x && iy < image_size.y) {
            const int left = min(point_a.pos.x, min(point_b.pos.x, point_c.pos.x));
            const int right = max(point_a.pos.x, max(point_b.pos.x, point_c.pos.x));

            if (left <= ix && ix <= right) {
                write_imagef(image, (int2)(ix, iy), (float4)(0.0f, 200.0f, 0.0f, 255.0));
            }
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

    kernel_ = compute::kernel(program_, "triangle_rasterization");
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
        break;
    }

    RunKernel(queue_, kernel_, view_size_, kRasterizeLocalSize);
}

void Rasterizer::FillVerticesInfo(const std::vector<InterpVertex>& points) {
    vertices_info_.clear();
    vertices_info_.reserve(points.size());
    for (auto [position, params] : points) {
        PerspectiveDivision(position);

        const cl_int2 pos = {
            .x = static_cast<cl_int>(std::floor(view_size_.x * (position.x() + 1.0) / 2.0)),
            .y = static_cast<cl_int>(std::floor(view_size_.y * (1.0 - position.y()) / 2.0)),
        };

        vertices_info_.push_back({.pos = pos});
    }
}

}  // namespace multithread::detail

}  // namespace null_engine
