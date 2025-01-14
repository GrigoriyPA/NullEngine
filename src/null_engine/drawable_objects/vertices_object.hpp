#pragma once

#include <vector>

#include "vertex.hpp"

namespace null_engine {

class VerticesObject {
public:
    explicit VerticesObject(uint64_t number_vertices);

    const std::vector<Vertex>& GetVertices() const;

    const std::vector<uint64_t>& GetIndices() const;

    VerticesObject& SetVertex(uint64_t index, const Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<Vec3>& positions);

    VerticesObject& SetColors(const std::vector<Vec3>& colors);

    VerticesObject& SetColor(Vec3 color);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

private:
    bool ValidateIdices(const std::vector<uint64_t>& indices) const;

private:
    std::vector<Vertex> vertices_;
    std::vector<uint64_t> indices_;
};

}  // namespace null_engine
