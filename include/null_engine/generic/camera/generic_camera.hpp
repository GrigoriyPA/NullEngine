#pragma once

#include <memory>

namespace null_engine::generic {

//
// Сommon camera interface, holds information about position in scene space,
// view direction and other projection settings
//
class Camera {
public:
    using Ptr = std::shared_ptr<Camera>;
};

}  // namespace null_engine::generic
