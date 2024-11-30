#pragma once

#include <memory>

namespace null_engine::generic {

//
// Common interface for all drawable objects
//
class MeshObject {
public:
    using Ptr = std::shared_ptr<MeshObject>;
};

}  // namespace null_engine::generic
