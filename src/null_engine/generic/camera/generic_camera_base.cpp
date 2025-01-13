#include "generic_camera_base.hpp"

#include <cassert>
#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::generic {

CameraBase::CameraBase()
    : position_()
    , direction_(0.0, 0.0, 1.0)
    , horizon_(1.0, 0.0, 0.0) {
}

util::Vec3 CameraBase::GetPosition() const {
    return position_;
}

util::Vec3 CameraBase::GetDirection() const {
    return direction_.Normalized();
}

util::Vec3 CameraBase::GetHorizon() const {
    return horizon_.Normalized();
}

util::Vec3 CameraBase::GetVertical() const {
    return horizon_.VectorProd(direction_).Normalize();
}

util::Transform CameraBase::GetCameraTransform() const {
    const auto vertical = horizon_.VectorProd(direction_);

    return util::Transform::Basis(horizon_, vertical, direction_)
        .Transpose()
        .ComposeBefore(util::Transform::Translation(-position_));
}

CameraBase& CameraBase::SetPosition(util::Vec3 position) {
    position_ = position;

    return *this;
}

CameraBase& CameraBase::SetDirection(util::Vec3 direction) {
    direction_ = direction;
    horizon_ = direction_.Horizon();

    return *this;
}

CameraBase& CameraBase::SetOrientation(util::Vec3 direction, util::Vec3 horizon) {
    assert(util::Equal(direction.ScalarProd(horizon), 0.0) && "Direction and horizon should be orthogonal");

    direction_ = direction;
    horizon_ = horizon;

    return *this;
}

void CameraBase::Move(util::Vec3 translation) {
    position_ += translation;
}

void CameraBase::Rotate(util::Vec3 axis, util::FloatType angle) {
    const auto transform = util::Transform::Rotation(axis, angle);

    direction_ = transform.Apply(direction_);
    horizon_ = transform.Apply(horizon_);
}

}  // namespace null_engine::generic
