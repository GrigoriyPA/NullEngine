#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>

namespace null_engine::detail {

struct ClippingResult {
    std::vector<Vertex> vertices;
    std::vector<TriangleIndex> indices;
};

class Clipper {
public:
    ClippingResult ClipTriangles(std::vector<Vertex> vertices, std::vector<TriangleIndex> indices) const;
};

}  // namespace null_engine::detail
