#pragma once

#include "native_rasterizer_context.hpp"

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

#include <null_engine/native/common/native_vertex.hpp>

namespace null_engine::native {

//
// Rasterizer can draw points / lines / triangles in current buffers,
// All coordinates expected in box [-1, 1] for each dimension
//
class Rasterizer {
public:
    explicit Rasterizer(RasterizerContext& context);

    void DrawPoint(Vertex point);

private:
    // Returns point coordinates on view
    std::pair<int64_t, int64_t> ProjectPoint(const Vertex& point) const;

    // Perform depth test
    bool CheckPointDepth(int64_t x, int64_t y, const Vertex& point) const;

    // Update color and depth buffers
    void UpdateViewPixel(int64_t x, int64_t y, const Vertex& point);

private:
    RasterizerContext& context_;
    const util::FloatType pixel_height_;
    const util::FloatType pixel_width_;
};

namespace tests {

void DrawPoints(native::Rasterizer& rasterizer, uint64_t number_points, util::Vec3 offset = util::Vec3(-0.5, -0.5),
                util::Vec3 size = util::Vec3(1.0, 1.0), util::Vec3 color = util::Vec3(1.0));

}  // namespace tests

}  // namespace null_engine::native
