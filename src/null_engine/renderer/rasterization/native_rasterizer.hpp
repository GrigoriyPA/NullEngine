#pragma once

#include <null_engine/renderer/shaders/fragment_shader.hpp>

#include "interpolation.hpp"

namespace null_engine::native::detail {

struct RasterizerBuffer {
    std::vector<uint8_t> colors;
    std::vector<FloatType> depth;
};

class Rasterizer {
    using InterpVertex = null_engine::detail::InterpVertex;

public:
    Rasterizer(uint64_t view_width, uint64_t view_height);

    void DrawPoint(const InterpVertex& point, RasterizerBuffer& buffer, const FragmentShader& shader) const;

    void DrawLine(
        const InterpVertex& point_a, const InterpVertex& point_b, RasterizerBuffer& buffer, const FragmentShader& shader
    ) const;

    void DrawTriangle(
        const InterpVertex& point_a, const InterpVertex& point_b, const InterpVertex& point_c, RasterizerBuffer& buffer,
        const FragmentShader& shader
    ) const;

private:
    VertexInfo GetVertexInfo(const InterpVertex& point) const;

    void RasterizeHorizontalLine(HorizontalLine line, RasterizerBuffer& buffer, const FragmentShader& shader) const;

    void RasterizePoint(const VertexInfo& vertex_info, RasterizerBuffer& buffer, const FragmentShader& shader) const;

    bool CheckPointPosition(int64_t x, int64_t y) const;

    bool CheckPointDepth(int64_t x, int64_t y, FloatType z, RasterizerBuffer& buffer) const;

    void UpdateViewPixel(const VertexInfo& vertex_info, RasterizerBuffer& buffer, const FragmentShader& shader) const;

    uint64_t view_width_;
    uint64_t view_height_;
};

}  // namespace null_engine::native::detail
