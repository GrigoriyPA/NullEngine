#include "rasterizer.hpp"

#include <boost/compute/utility/dim.hpp>
#include <boost/compute/utility/source.hpp>
#include <null_engine/util/generic/helpers.hpp>

namespace null_engine {

namespace native::detail {

namespace {

class LineWalker {
    class LinePos {
    public:
        int64_t d = 0;
        int64_t x = 0;
        int64_t y = 0;

        LinePos& operator+=(const LinePos& other) {
            d += other.d;
            x += other.x;
            y += other.y;
            return *this;
        }
    };

public:
    LineWalker(const VertexInfo& start, const VertexInfo& end)
        : dx_(std::abs(end.x - start.x))
        , dy_(std::abs(end.y - start.y))
        , interpolation_(start.interpolation, end.interpolation, dx_ + dy_ + 1)
        , current_pos_({.d = 0, .x = start.x, .y = start.y}) {
        const auto sx = end.x >= start.x ? 1 : -1;
        const auto sy = end.y >= start.y ? 1 : -1;
        if (dy_ <= dx_) {
            step1_ = {.d = 2 * dy_, .x = sx};
            step2_ = {.d = -2 * dx_, .y = sy};
        } else {
            step1_ = {.d = 2 * dx_, .y = sy};
            step2_ = {.d = -2 * dy_, .x = sx};
        }
    }

    bool Finished() const {
        return step_index_ > dx_ + dy_;
    }

    VertexInfo GetVertex() const {
        return {.x = current_pos_.x, .y = current_pos_.y, .interpolation = interpolation_.Get()};
    }

    void Move() {
        if (!Finished()) {
            MakeStep(GetNextStep());
        }
    }

    void MoveHorizontal() {
        while (!Finished()) {
            const auto& step = GetNextStep();
            if (step.y != 0) {
                break;
            }
            MakeStep(step);
        }
    }

private:
    const LinePos& GetNextStep() const {
        return current_pos_.d > 0 ? step2_ : step1_;
    }

    void MakeStep(const LinePos& step) {
        assert(!Finished() && "Too many steps performed");

        current_pos_ += step;
        interpolation_.Increment();
        ++step_index_;
    }

    int64_t dx_;
    int64_t dy_;
    DirValue<Interpolation> interpolation_;
    LinePos step1_;
    LinePos step2_;
    LinePos current_pos_;
    int64_t step_index_ = 0;
};

void PerspectiveDivision(Vec4& position) {
    position.x() /= position.w();
    position.y() /= position.w();
    position.z() /= position.w();
    position.w() = 1.0 / position.w();
}

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height)
    : view_width_(view_width)
    , view_height_(view_height)
    , pixel_height_(2.0 / static_cast<FloatType>(view_height))
    , pixel_width_(2.0 / static_cast<FloatType>(view_width)) {
}

void Rasterizer::DrawPoint(const InterpVertex& point, RasterizerBuffer& buffer, const FragmentShader& shader) const {
    RasterizePoint(GetVertexInfo(point), buffer, shader);
}

void Rasterizer::DrawLine(
    const InterpVertex& point_a, const InterpVertex& point_b, RasterizerBuffer& buffer, const FragmentShader& shader
) const {
    LineWalker walker(GetVertexInfo(point_a), GetVertexInfo(point_b));
    for (; !walker.Finished(); walker.Move()) {
        RasterizePoint(walker.GetVertex(), buffer, shader);
    }
}

void Rasterizer::DrawTriangle(
    const InterpVertex& point_a, const InterpVertex& point_b, const InterpVertex& point_c, RasterizerBuffer& buffer,
    const FragmentShader& shader
) const {
    auto info_a = GetVertexInfo(point_a);
    auto info_b = GetVertexInfo(point_b);
    auto info_c = GetVertexInfo(point_c);

    SortValues<VertexInfo>(info_a, info_b, info_c, [](const auto& left, const auto& right) {
        return left.y > right.y;
    });

    if (info_a.y == info_c.y) {
        SortValues<VertexInfo>(info_a, info_b, info_c, [](const auto& left, const auto& right) {
            return left.x < right.x;
        });
        RasterizeHorizontalLine(HorizontalLine(info_a, info_c), buffer, shader);
        return;
    }

    LineWalker walker_ac(info_a, info_c);

    if (info_a.y < info_b.y) {
        LineWalker walker_ab(info_a, info_b);
        for (; !walker_ac.Finished() && !walker_ab.Finished(); walker_ac.Move(), walker_ab.Move()) {
            walker_ac.MoveHorizontal();
            walker_ab.MoveHorizontal();

            HorizontalLine line(walker_ac.GetVertex(), walker_ab.GetVertex());
            RasterizeHorizontalLine(line, buffer, shader);
        }
    }

    if (info_b.y < info_c.y) {
        LineWalker walker_bc(info_b, info_c);
        while (!walker_ac.Finished() && walker_ac.GetVertex().y < walker_bc.GetVertex().y) {
            walker_ac.Move();
        }

        while (!walker_bc.Finished() && walker_bc.GetVertex().y < walker_ac.GetVertex().y) {
            walker_bc.Move();
        }

        for (; !walker_ac.Finished() && !walker_bc.Finished(); walker_ac.Move(), walker_bc.Move()) {
            walker_ac.MoveHorizontal();
            walker_bc.MoveHorizontal();

            HorizontalLine line(walker_ac.GetVertex(), walker_bc.GetVertex());
            RasterizeHorizontalLine(line, buffer, shader);
        }
    }
}

VertexInfo Rasterizer::GetVertexInfo(const InterpVertex& point) const {
    auto position = point.position;
    PerspectiveDivision(position);

    return {
        .x = static_cast<int64_t>(std::floor((position.x() + 1.0) / pixel_width_)),
        .y = static_cast<int64_t>(std::floor((1.0 - position.y()) / pixel_height_)),
        .interpolation = Interpolation(position.z(), position.w(), point.params)
    };
}

void Rasterizer::RasterizeHorizontalLine(HorizontalLine line, RasterizerBuffer& buffer, const FragmentShader& shader)
    const {
    for (; !line.Finished(); line.Increment()) {
        RasterizePoint(line.GetVertex(), buffer, shader);
    }
}

void Rasterizer::RasterizePoint(const VertexInfo& vertex_info, RasterizerBuffer& buffer, const FragmentShader& shader)
    const {
    if (!CheckPointPosition(vertex_info.x, vertex_info.y)) {
        return;
    }

    if (!CheckPointDepth(vertex_info.x, vertex_info.y, vertex_info.interpolation.GetZ(), buffer)) {
        return;
    }

    UpdateViewPixel(vertex_info, buffer, shader);
}

bool Rasterizer::CheckPointPosition(int64_t x, int64_t y) const {
    return 0 <= x && x < view_width_ && 0 <= y && y < view_height_;
}

bool Rasterizer::CheckPointDepth(int64_t x, int64_t y, FloatType z, RasterizerBuffer& buffer) const {
    if (z <= -1.0 || 1.0 <= z) {
        return false;
    }
    return buffer.depth[y * view_width_ + x] > z;
}

void Rasterizer::UpdateViewPixel(const VertexInfo& vertex_info, RasterizerBuffer& buffer, const FragmentShader& shader)
    const {
    const uint64_t point_offset = vertex_info.y * view_width_ + vertex_info.x;
    buffer.depth[point_offset] = vertex_info.interpolation.GetZ();

    auto color = shader.GetPointColor(vertex_info.interpolation.GetParams());
    color = (color * 255.0).cwiseMax(0.0).cwiseMin(255.0);

    buffer.colors[4 * point_offset] = static_cast<uint8_t>(color.x());
    buffer.colors[4 * point_offset + 1] = static_cast<uint8_t>(color.y());
    buffer.colors[4 * point_offset + 2] = static_cast<uint8_t>(color.z());
    buffer.colors[4 * point_offset + 3] = 255;
}

}  // namespace native::detail

namespace multithread::detail {

const std::string kSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
    // map value to color
    float4 color(uint i) {
        uchar c = i;
        uchar x = 35;
        uchar y = 25;
        uchar z = 15;
        uchar max = 255;

        if (i == 256)
            return (float4)(0, 0, 0, 255);
        else
            return (float4)(max - x * i, max - y * i, max - z * i, max) / 255.0f;
    }

    __kernel void mandelbrot(__write_only image2d_t image) {
        const uint x_coord = get_global_id(0);
        const uint y_coord = get_global_id(1);
        const uint width = get_global_size(0);
        const uint height = get_global_size(1);

        float x_origin = ((float)x_coord / width) * 3.25f - 2.0f;
        float y_origin = ((float)y_coord / height) * 2.5f - 1.25f;

        float x = 0.0f;
        float y = 0.0f;

        uint i = 0;
        while (x * x + y * y <= 4.0f && i < 256) {
            float tmp = x * x - y * y + x_origin;
            y = 2 * x * y + y_origin;
            x = tmp;
            i++;
        }

        int2 coord = {x_coord, y_coord};
        write_imagef(image, coord, color(i));
    };
);

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height, compute::context context)
    : view_width_(view_width)
    , view_height_(view_height)
    , context_(std::move(context))
    , program_(compute::program::create_with_source(kSource, context_)) {
    program_.build();

    kernel_ = compute::kernel(program_, "mandelbrot");
}

void Rasterizer::DrawTriangles(
    const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
) {
    kernel_.set_arg(0, buffer.colors);

    buffer.queue.enqueue_nd_range_kernel(
        kernel_, compute::dim(0, 0), compute::dim(view_width_, view_height_), compute::dim(1, 1)
    );
}

}  // namespace multithread::detail

}  // namespace null_engine
