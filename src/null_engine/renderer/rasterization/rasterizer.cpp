#include "rasterizer.hpp"

#include <cassert>
#include <null_engine/util/generic/helpers.hpp>

namespace null_engine::detail {

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
    position.X() /= position.H();
    position.Y() /= position.H();
    position.Z() /= position.H();
    position.H() = 1.0 / position.H();
}

}  // anonymous namespace

Rasterizer::Rasterizer(uint64_t view_width, uint64_t view_height)
    : view_width_(view_width)
    , view_height_(view_height)
    , pixel_height_(2.0 / static_cast<FloatType>(view_height))
    , pixel_width_(2.0 / static_cast<FloatType>(view_width)) {
}

void Rasterizer::DrawPoint(const Vertex& point, RasterizerBuffer& buffer) const {
    RasterizePoint(GetVertexInfo(point), buffer);
}

void Rasterizer::DrawLine(const Vertex& point_a, const Vertex& point_b, RasterizerBuffer& buffer) const {
    LineWalker walker(GetVertexInfo(point_a), GetVertexInfo(point_b));
    for (; !walker.Finished(); walker.Move()) {
        RasterizePoint(walker.GetVertex(), buffer);
    }
}

void Rasterizer::DrawTriangle(
    const Vertex& point_a, const Vertex& point_b, const Vertex& point_c, RasterizerBuffer& buffer
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
        RasterizeHorizontalLine(HorizontalLine(info_a, info_c), buffer);
        return;
    }

    LineWalker walker_ac(info_a, info_c);

    if (info_a.y < info_b.y) {
        LineWalker walker_ab(info_a, info_b);
        for (; !walker_ac.Finished() && !walker_ab.Finished(); walker_ac.Move(), walker_ab.Move()) {
            walker_ac.MoveHorizontal();
            walker_ab.MoveHorizontal();

            RasterizeHorizontalLine(HorizontalLine(walker_ac.GetVertex(), walker_ab.GetVertex()), buffer);
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

            RasterizeHorizontalLine(HorizontalLine(walker_ac.GetVertex(), walker_bc.GetVertex()), buffer);
        }
    }
}

VertexInfo Rasterizer::GetVertexInfo(const Vertex& point) const {
    auto position = point.position;
    PerspectiveDivision(position);

    return {
        .x = static_cast<int64_t>(std::floor((position.X() + 1.0) / pixel_width_)),
        .y = static_cast<int64_t>(std::floor((1.0 - position.Y()) / pixel_height_)),
        .interpolation = Interpolation(position.Z(), position.H(), point.params)
    };
}

void Rasterizer::RasterizeHorizontalLine(HorizontalLine line, RasterizerBuffer& buffer) const {
    for (; !line.Finished(); line.Increment()) {
        RasterizePoint(line.GetVertex(), buffer);
    }
}

void Rasterizer::RasterizePoint(const VertexInfo& vertex_info, RasterizerBuffer& buffer) const {
    if (!CheckPointPosition(vertex_info.x, vertex_info.y)) {
        return;
    }

    if (!CheckPointDepth(vertex_info.x, vertex_info.y, vertex_info.interpolation.GetZ(), buffer)) {
        return;
    }

    UpdateViewPixel(vertex_info, buffer);
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

void Rasterizer::UpdateViewPixel(const VertexInfo& vertex_info, RasterizerBuffer& buffer) const {
    const uint64_t point_offset = vertex_info.y * view_width_ + vertex_info.x;
    buffer.depth[point_offset] = vertex_info.interpolation.GetZ();

    const auto params = vertex_info.interpolation.GetParams();
    const Vec3 color = (params.color * 255.0).Clamp(0.0, 255.0);
    buffer.colors[4 * point_offset] = static_cast<uint8_t>(color.X());
    buffer.colors[4 * point_offset + 1] = static_cast<uint8_t>(color.Y());
    buffer.colors[4 * point_offset + 2] = static_cast<uint8_t>(color.Z());
    buffer.colors[4 * point_offset + 3] = 255;
}

}  // namespace null_engine::detail
