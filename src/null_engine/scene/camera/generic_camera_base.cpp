#include "generic_camera_base.hpp"

#include <cassert>
#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine {

CameraBase::CameraBase()
    : position_()
    , direction_(0.0, 0.0, 1.0)
    , horizon_(1.0, 0.0, 0.0) {
}

Vec3 CameraBase::GetPosition() const {
    return position_;
}

Vec3 CameraBase::GetDirection() const {
    return direction_.Normalized();
}

Vec3 CameraBase::GetHorizon() const {
    return horizon_.Normalized();
}

Vec3 CameraBase::GetVertical() const {
    return horizon_.VectorProd(direction_).Normalize();
}

Transform CameraBase::GetCameraTransform() const {
    const auto vertical = horizon_.VectorProd(direction_);

    return Transform::Basis(horizon_, vertical, direction_)
        .Transpose()
        .ComposeBefore(Transform::Translation(-position_));
}

CameraBase& CameraBase::SetPosition(Vec3 position) {
    position_ = position;

    return *this;
}

CameraBase& CameraBase::SetDirection(Vec3 direction) {
    direction_ = direction;
    horizon_ = direction_.Horizon();

    return *this;
}

CameraBase& CameraBase::SetOrientation(Vec3 direction, Vec3 horizon) {
    assert(Equal(direction.ScalarProd(horizon), 0.0) && "Direction and horizon should be orthogonal");

    direction_ = direction;
    horizon_ = horizon;

    return *this;
}

void CameraBase::Move(Vec3 translation) {
    position_ += translation;
}

void CameraBase::Rotate(Vec3 axis, FloatType angle) {
    const auto transform = Transform::Rotation(axis, angle);

    direction_ = transform.Apply(direction_);
    horizon_ = transform.Apply(horizon_);
}

}  // namespace null_engine
