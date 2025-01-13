#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/transformation.hpp>

#include "generic_camera_base.hpp"

namespace null_engine::generic {

class PerspectiveCamera : public CameraBase {
public:
    PerspectiveCamera(
        util::FloatType fov, util::FloatType ratio, util::FloatType min_distance, util::FloatType max_distance
    );

    util::Transform GetNdcTransform() const;

private:
    const util::Transform ndc_transform_;
};

}  // namespace null_engine::generic
