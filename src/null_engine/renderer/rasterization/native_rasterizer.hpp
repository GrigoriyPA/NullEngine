#pragma once

#include <null_engine/renderer/shaders/fragment_shader_interface.hpp>

#include "interpolation.hpp"

namespace null_engine::native::detail {

struct RasterizerBuffer {
    std::vector<uint8_t> colors;
    std::vector<float> depth;
};

class Rasterizer {
    using InterpVertex = null_engine::detail::InterpVertex;

public:
    struct ViewInfo {
        uint64_t width;
        uint64_t height;
    };

    explicit Rasterizer(ViewInfo view);

    void UpdateView(ViewInfo view);

    void DrawPoint(const InterpVertex& point, RasterizerBuffer& buffer, const AnyFragmentShaderRef& shader) const;

    void DrawLine(
        const InterpVertex& point_a, const InterpVertex& point_b, RasterizerBuffer& buffer,
        const AnyFragmentShaderRef& shader
    ) const;

    void DrawTriangle(
        const InterpVertex& point_a, const InterpVertex& point_b, const InterpVertex& point_c, RasterizerBuffer& buffer,
        const AnyFragmentShaderRef& shader
    ) const;

private:
    VertexInfo GetVertexInfo(const InterpVertex& point) const;

    void RasterizeHorizontalLine(HorizontalLine line, RasterizerBuffer& buffer, const AnyFragmentShaderRef& shader)
        const;

    void RasterizePoint(const VertexInfo& vertex_info, RasterizerBuffer& buffer, const AnyFragmentShaderRef& shader)
        const;

    bool CheckPointPosition(int64_t x, int64_t y) const;

    bool CheckPointDepth(int64_t x, int64_t y, float z, RasterizerBuffer& buffer) const;

    void UpdateViewPixel(const VertexInfo& vertex_info, RasterizerBuffer& buffer, const AnyFragmentShaderRef& shader)
        const;

    uint64_t view_width_;
    uint64_t view_height_;
};

}  // namespace null_engine::native::detail
