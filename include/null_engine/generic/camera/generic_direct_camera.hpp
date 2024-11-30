#pragma once

#include "generic_camera_base.hpp"

#include <null_engine/util/generic/templates.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::generic {

//
// Camera which performs direct projection without perspective transform
//
class DirectCamera : public CameraBase, public util::SharedConstructable<DirectCamera> {
public:
    using Ptr = std::shared_ptr<DirectCamera>;

public:
    // Box size should be strictly positive
    DirectCamera(util::FloatType width, util::FloatType height, util::FloatType depth);

public:
    util::Transform GetNdcTransform() const override;

private:
    // View box size relative to scene coordinates
    util::FloatType width_ = 0.0;
    util::FloatType height_ = 0.0;
    util::FloatType depth_ = 0.0;

    // Transformm from camera space to NDC space
    util::Transform ndc_transform_;
};

}  // namespace null_engine::generic
