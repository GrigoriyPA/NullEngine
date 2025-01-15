#pragma once

#include <vector>

#include "vertex.hpp"

namespace null_engine {

class VerticesObject {
public:
    enum class Type { Points, Triangles };

    explicit VerticesObject(uint64_t number_vertices, Type object_type = Type::Points);

    Type GetObjectType() const;

    const std::vector<Vertex>& GetVertices() const;

    const std::vector<uint64_t>& GetIndices() const;

    VerticesObject& SetVertex(uint64_t index, const Vertex& vertex);

    VerticesObject& SetPositions(const std::vector<Vec3>& positions);

    VerticesObject& SetParams(const std::vector<VertexParams>& params);

    VerticesObject& SetParams(const VertexParams& params);

    VerticesObject& SetIndices(const std::vector<uint64_t>& indices);

private:
    bool ValidateIdicesValues(const std::vector<uint64_t>& indices) const;

    void FillDefaultIndices(size_t indices_size);

    Type object_type_;
    std::vector<Vertex> vertices_;
    std::vector<uint64_t> indices_;
};

}  // namespace null_engine
