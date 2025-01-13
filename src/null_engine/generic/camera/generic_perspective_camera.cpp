#include "generic_perspective_camera.hpp"

#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::generic {

PerspectiveCamera::PerspectiveCamera(
    util::FloatType fov, util::FloatType ratio, util::FloatType min_distance, util::FloatType max_distance
)
    : ndc_transform_(util::Transform::PerspectiveProjection(fov, ratio, min_distance, max_distance)) {
}

util::Transform PerspectiveCamera::GetNdcTransform() const {
    return GetCameraTransform().ComposeAfter(ndc_transform_);
}

}  // namespace null_engine::generic
