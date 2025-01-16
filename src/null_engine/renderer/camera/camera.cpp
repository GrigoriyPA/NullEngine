#include "camera.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

namespace detail {

CameraBase::CameraBase()
    : position_()
    , direction_(0.0, 0.0, 1.0)
    , horizon_(1.0, 0.0, 0.0) {
}

Vec3 CameraBase::GetPosition() const {
    return position_;
}

Vec3 CameraBase::GetDirection() const {
    return Vec3::Normalize(direction_);
}

Vec3 CameraBase::GetHorizon() const {
    return Vec3::Normalize(horizon_);
}

Vec3 CameraBase::GetVertical() const {
    return horizon_.VectorProd(direction_).Normalize();
}

Mat4 CameraBase::GetCameraTransform() const {
    const auto vertical = horizon_.VectorProd(direction_);

    return Mat4::Basis(horizon_, vertical, direction_).Transpose() * Mat4::Translation(-position_);
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

void CameraBase::MoveGlobal(Vec3 translation) {
    position_ += translation;
}

void CameraBase::Move(FloatType direct_move, FloatType horizon_move, FloatType vertical_move) {
    MoveGlobal(direction_ * direct_move + horizon_ * horizon_move + GetVertical() * vertical_move);
}

void CameraBase::RotateGlobal(Vec3 axis, FloatType angle) {
    const auto transform = Mat4::Rotation(axis, angle);

    direction_ = transform.Apply(direction_);
    horizon_ = transform.Apply(horizon_);
}

void CameraBase::Rotate(FloatType yaw_rotation, FloatType pitch_rotation, FloatType roll_rotation) {
    RotateGlobal(GetVertical(), yaw_rotation);
    RotateGlobal(horizon_, pitch_rotation);
    RotateGlobal(direction_, roll_rotation);
}

}  // namespace detail

DirectCamera::DirectCamera(FloatType width, FloatType height, FloatType depth)
    : ndc_transform_(Mat4::BoxProjection(width, height, depth)) {
}

Mat4 DirectCamera::GetNdcTransform() const {
    return ndc_transform_ * GetCameraTransform();
}

PerspectiveCamera::PerspectiveCamera(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance)
    : ndc_transform_(Mat4::PerspectiveProjection(fov, ratio, min_distance, max_distance)) {
}

Mat4 PerspectiveCamera::GetNdcTransform() const {
    return ndc_transform_ * GetCameraTransform();
}

}  // namespace null_engine
