#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/transformation.hpp>

#include "generic_camera_base.hpp"

namespace null_engine {

class DirectCamera : public CameraBase {
public:
    DirectCamera(FloatType width, FloatType height, FloatType depth);

    Transform GetNdcTransform() const;

private:
    const Transform ndc_transform_;
};

}  // namespace null_engine
