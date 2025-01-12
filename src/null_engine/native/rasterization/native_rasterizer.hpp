#pragma once

#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

#include "native_rasterizer_context.hpp"

namespace null_engine::native {

//
// Rasterizer can draw points / lines / triangles in current buffers,
// All coordinates expected in box [-1, 1] for each dimension
//
class Rasterizer {
public:
    explicit Rasterizer(RasterizerContext& context);

    void DrawPoint(generic::Vertex point);

private:
    std::pair<int64_t, int64_t> ProjectPoint(const generic::Vertex& point) const;

    bool CheckPointDepth(int64_t x, int64_t y, const generic::Vertex& point) const;

    void UpdateViewPixel(int64_t x, int64_t y, const generic::Vertex& point);

private:
    RasterizerContext& context_;
    const util::FloatType pixel_height_;
    const util::FloatType pixel_width_;
};

namespace tests {

void DrawPoints(
    Rasterizer& rasterizer, uint64_t number_points, util::Vec3 offset = util::Vec3(-0.5, -0.5),
    util::Vec3 size = util::Vec3(1.0, 1.0), util::Vec3 color = util::Vec3(1.0)
);

}  // namespace tests

}  // namespace null_engine::native
