#pragma once

#include <null_engine/util/geometry/transformation.hpp>

#include <memory>

namespace null_engine::generic {

//
// Сommon camera interface, holds information about position in scene space,
// view direction and other projection settings
//
class Camera {
public:
    using Ptr = std::shared_ptr<Camera>;

public:
    // Transform from scene space to normalized device coordinates:
    // -- NDC coordinates is box [-1, 1] for all coordinates
    virtual util::Transform GetNdcTransform() const = 0;
};

}  // namespace null_engine::generic
