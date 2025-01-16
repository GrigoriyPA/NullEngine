#include "clipping.hpp"

namespace null_engine::detail {

ClippingResult Clipper::ClipTriangles(std::vector<Vertex> vertices, std::vector<TriangleIndex> indices) const {
    ClippingResult result{.vertices = std::move(vertices), .indices = std::move(indices)};

    return result;
}

}  // namespace null_engine::detail
