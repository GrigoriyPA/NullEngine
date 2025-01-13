#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/transformation.hpp>

#include "generic_camera_base.hpp"

namespace null_engine::generic {

class DirectCamera : public CameraBase {
public:
    DirectCamera(util::FloatType width, util::FloatType height, util::FloatType depth);

    util::Transform GetNdcTransform() const;

private:
    const util::Transform ndc_transform_;
};

}  // namespace null_engine::generic
