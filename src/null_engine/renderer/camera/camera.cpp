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
    return direction_.Normalized();
}

Vec3 CameraBase::GetHorizon() const {
    return horizon_.Normalized();
}

Vec3 CameraBase::GetVertical() const {
    return horizon_.VectorProd(direction_).Normalize();
}

Mat4 CameraBase::GetCameraMat4() const {
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

void CameraBase::Move(Vec3 translation) {
    position_ += translation;
}

void CameraBase::Rotate(Vec3 axis, FloatType angle) {
    const auto transform = Mat4::Rotation(axis, angle);

    direction_ = transform.Apply(direction_);
    horizon_ = transform.Apply(horizon_);
}

}  // namespace detail

DirectCamera::DirectCamera(FloatType width, FloatType height, FloatType depth)
    : ndc_transform_(Mat4::BoxProjection(width, height, depth)) {
}

Mat4 DirectCamera::GetNdcMat4() const {
    return ndc_transform_ * GetCameraMat4();
}

PerspectiveCamera::PerspectiveCamera(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance)
    : ndc_transform_(Mat4::PerspectiveProjection(fov, ratio, min_distance, max_distance)) {
}

Mat4 PerspectiveCamera::GetNdcMat4() const {
    return ndc_transform_ * GetCameraMat4();
}

}  // namespace null_engine
