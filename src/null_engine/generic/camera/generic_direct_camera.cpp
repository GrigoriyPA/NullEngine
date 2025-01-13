#include "generic_direct_camera.hpp"

#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::generic {

DirectCamera::DirectCamera(util::FloatType width, util::FloatType height, util::FloatType depth)
    : ndc_transform_(util::Transform::BoxProjection(width, height, depth)) {
}

util::Transform DirectCamera::GetNdcTransform() const {
    return GetCameraTransform().ComposeAfter(ndc_transform_);
}

}  // namespace null_engine::generic
