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

    void DrawPoint(generic::Vertex point);

private:
    std::pair<int64_t, int64_t> ProjectPoint(const generic::Vertex& point) const;

    bool CheckPointDepth(int64_t x, int64_t y, const generic::Vertex& point) const;

    void UpdateViewPixel(int64_t x, int64_t y, const generic::Vertex& point);

private:
    RasterizerContext context_;
    const util::FloatType pixel_height_;
    const util::FloatType pixel_width_;
};

}  // namespace null_engine::native
