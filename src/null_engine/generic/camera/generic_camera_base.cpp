#include <null_engine/generic/camera/generic_camera_base.hpp>

#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::generic {

//// CameraBase

CameraBase::CameraBase()
    : position_()
    , direction_(0.0, 0.0, 1.0)
    , horizon_(1.0, 0.0, 0.0)
    , camera_transform_() {
}

CameraBase& CameraBase::SetPosition(util::Vec3 position) {
    position_ = position;
    ResetCameraTransform();

    return *this;
}

CameraBase& CameraBase::SetDirection(util::Vec3 direction) {
    direction_ = direction;
    horizon_ = direction_.Horizon();
    ResetCameraTransform();

    return *this;
}

CameraBase& CameraBase::SetOrientation(util::Vec3 direction, util::Vec3 horizon) {
    util::Ensure(util::Equal(direction.ScalarProd(horizon), 0.0), "Direction and horizon should be orthogonal");

    direction_ = direction;
    horizon_ = horizon;
    ResetCameraTransform();

    return *this;
}

void CameraBase::Move(util::Vec3 translation) {
    position_ += translation;
    camera_transform_.ComposeAfter(util::Transform::Translation(-translation));
}

void CameraBase::Rotate(util::Vec3 axis, util::FloatType angle) {
    const auto transform = util::Transform::Rotation(axis, angle);

    position_ = transform.Apply(position_);
    direction_ = transform.Apply(direction_);
    horizon_ = transform.Apply(horizon_);

    camera_transform_.ComposeAfter(transform.Transposed());
}

void CameraBase::ResetCameraTransform() {
    const auto vertical = horizon_.VectorProd(direction_);  // Right hand vertical

    camera_transform_ = util::Transform::Basis(horizon_, vertical, direction_)
                            .Transpose()  // Inverse basis matrix
                            .ComposeBefore(util::Transform::Translation(-position_));
}

}  // namespace null_engine::generic
