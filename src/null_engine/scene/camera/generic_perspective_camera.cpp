#include "generic_perspective_camera.hpp"

#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine {

PerspectiveCamera::PerspectiveCamera(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance)
    : ndc_transform_(Transform::PerspectiveProjection(fov, ratio, min_distance, max_distance)) {
}

Transform PerspectiveCamera::GetNdcTransform() const {
    return GetCameraTransform().ComposeAfter(ndc_transform_);
}

}  // namespace null_engine
