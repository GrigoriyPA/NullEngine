#include "generic_direct_camera.hpp"

#include <null_engine/util/geometry/transformation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::generic {

DirectCamera::DirectCamera(util::FloatType width, util::FloatType height, util::FloatType depth)
    : width_(width)
    , height_(height)
    , depth_(depth)
    , ndc_transform_(util::Transform::BoxProjection(width_, height_, depth_)) {
}

util::Transform DirectCamera::GetNdcTransform() const {
    return GetCameraTransform().ComposeAfter(ndc_transform_);
}

}  // namespace null_engine::generic
