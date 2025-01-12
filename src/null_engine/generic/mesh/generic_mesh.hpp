#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "generic_vertex.hpp"

namespace null_engine::generic {

//
// Common interface for all drawable objects
//
class MeshObject {
public:
    using Ptr = std::shared_ptr<MeshObject>;

public:
    virtual const std::vector<Vertex>& GetVertices() const = 0;

    virtual const std::vector<uint64_t>& GetIndices() const = 0;
};

}  // namespace null_engine::generic
