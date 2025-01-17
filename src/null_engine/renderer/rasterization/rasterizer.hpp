#pragma once

#include <vector>

#include "interpolation.hpp"

namespace null_engine::detail {

struct RasterizerBuffer {
    std::vector<uint8_t> colors;
    std::vector<FloatType> depth;
};

class Rasterizer {
public:
    Rasterizer(uint64_t view_width, uint64_t view_height);

    void DrawPoint(const Vertex& point, RasterizerBuffer& buffer) const;

    void DrawLine(const Vertex& point_a, const Vertex& point_b, RasterizerBuffer& buffer) const;

    void DrawTriangle(const Vertex& point_a, const Vertex& point_b, const Vertex& point_c, RasterizerBuffer& buffer)
        const;

private:
    VertexInfo GetVertexInfo(const Vertex& point) const;

    void RasterizeHorizontalLine(HorizontalLine line, RasterizerBuffer& buffer) const;

    void RasterizePoint(const VertexInfo& vertex_info, RasterizerBuffer& buffer) const;

    bool CheckPointPosition(int64_t x, int64_t y) const;

    bool CheckPointDepth(int64_t x, int64_t y, FloatType z, RasterizerBuffer& buffer) const;

    void UpdateViewPixel(const VertexInfo& vertex_info, RasterizerBuffer& buffer) const;

    uint64_t view_width_;
    uint64_t view_height_;
    FloatType pixel_height_;
    FloatType pixel_width_;
};

}  // namespace null_engine::detail
