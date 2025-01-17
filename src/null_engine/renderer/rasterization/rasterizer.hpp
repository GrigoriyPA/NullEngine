#pragma once

#include <vector>

#include "interpolation.hpp"

namespace null_engine::detail {

struct RasterizerBuffer {
    std::vector<uint8_t> colors;
    std::vector<FloatType> depth;
};

class Rasterizer {
    struct PixelPos {
        int64_t x;
        int64_t y;
    };

public:
    Rasterizer(uint64_t view_width, uint64_t view_height);

    void DrawPoint(Vertex point, RasterizerBuffer& buffer) const;

    void DrawLine(Vertex point_a, Vertex point_b, RasterizerBuffer& buffer) const;

    void DrawTriangle(Vertex point_a, Vertex point_b, Vertex point_c, RasterizerBuffer& buffer) const;

private:
    PixelPos GetPixelPos(const Vertex& point) const;

    void RasterizeTriangleHalf(TriangleBorders borders, RasterizerBuffer& buffer) const;

    void RasterizeHorizontalLine(RsteriztionLine line, RasterizerBuffer& buffer) const;

    void RasterizePoint(int64_t x, int64_t y, const Interpolation& interpolation, RasterizerBuffer& buffer) const;

    bool CheckPointPosition(int64_t x, int64_t y) const;

    bool CheckPointDepth(int64_t x, int64_t y, FloatType z, RasterizerBuffer& buffer) const;

    void UpdateViewPixel(int64_t x, int64_t y, FloatType z, const VertexParams& point_params, RasterizerBuffer& buffer)
        const;

    uint64_t view_width_;
    uint64_t view_height_;
    FloatType pixel_height_;
    FloatType pixel_width_;
};

}  // namespace null_engine::detail
