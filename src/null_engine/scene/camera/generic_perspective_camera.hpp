#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/transformation.hpp>

#include "generic_camera_base.hpp"

namespace null_engine {

class PerspectiveCamera : public CameraBase {
public:
    PerspectiveCamera(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance);

    Transform GetNdcTransform() const;

private:
    const Transform ndc_transform_;
};

}  // namespace null_engine
