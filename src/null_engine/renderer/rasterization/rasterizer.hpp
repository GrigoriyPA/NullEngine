#pragma once

#include <null_engine/drawable_objects/vertex.hpp>
#include <vector>

namespace null_engine {

struct RasterizerBuffer {
    std::vector<uint8_t> colors;
    std::vector<FloatType> depth;
};

class Rasterizer {
public:
    Rasterizer(uint64_t view_width, uint64_t view_height);

    void DrawPoint(const Vertex& point, RasterizerBuffer& buffer) const;

    void DrawTriangle(const Vertex& point_a, const Vertex& point_b, const Vertex& point_c, RasterizerBuffer& buffer)
        const;

private:
    bool CheckPointDepth(int64_t x, int64_t y, FloatType z, RasterizerBuffer& buffer) const;

    void UpdateViewPixel(int64_t x, int64_t y, FloatType z, const VertexParams& point_params, RasterizerBuffer& buffer)
        const;

    uint64_t view_width_;
    uint64_t view_height_;
    FloatType pixel_height_;
    FloatType pixel_width_;
};

}  // namespace null_engine
