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
    util::FloatType width_ = 0.0;
    util::FloatType height_ = 0.0;
    util::FloatType depth_ = 0.0;
    util::Transform ndc_transform_;
};

}  // namespace null_engine::generic
