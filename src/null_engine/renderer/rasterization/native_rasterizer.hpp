#pragma once

#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

#include "native_rasterizer_context.hpp"

namespace null_engine::native {

class Rasterizer {
public:
    explicit Rasterizer(RasterizerContext context);

    const RasterizerContext& GetContext() const;

    RasterizerContext& GetContext();

    void DrawPoint(Vertex point);

    void DrawTriangle(Vertex point_a, Vertex point_b, Vertex point_c);

private:
    std::pair<int64_t, int64_t> ProjectPoint(const Vertex& point) const;

    bool CheckPointDepth(int64_t x, int64_t y, FloatType z) const;

    void UpdateViewPixel(int64_t x, int64_t y, const Vertex& point);

private:
    RasterizerContext context_;
    const FloatType pixel_height_;
    const FloatType pixel_width_;
};

}  // namespace null_engine::native
