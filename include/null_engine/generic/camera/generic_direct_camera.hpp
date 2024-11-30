#pragma once

#include "generic_camera.hpp"

#include <null_engine/util/generic/templates.hpp>

namespace null_engine::generic {

//
// Camera which performs direct projection without perspective transform
//
class DirectCamera : public Camera, public util::SharedConstructable<DirectCamera> {
public:
    using Ptr = std::shared_ptr<DirectCamera>;
};

}  // namespace null_engine::generic
