#include "generic_direct_camera.hpp"

#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine {

DirectCamera::DirectCamera(FloatType width, FloatType height, FloatType depth)
    : ndc_transform_(Transform::BoxProjection(width, height, depth)) {
}

Transform DirectCamera::GetNdcTransform() const {
    return GetCameraTransform().ComposeAfter(ndc_transform_);
}

}  // namespace null_engine
