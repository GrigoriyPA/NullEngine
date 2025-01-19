#include "camera.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

namespace detail {

CameraBase::CameraBase(const CameraOrientation& orientation)
    : orientation_(orientation)
    , in_change_port_(std::bind(&CameraBase::OnCameraChange, this, std::placeholders::_1)) {
}

InPort<CameraChange>* CameraBase::GetChangePort() {
    return &in_change_port_;
}

void CameraBase::SubscribeOnCameraTransform(InPort<Mat4>* observer_port) const {
    out_transform_port_->Subscribe(observer_port, GetCameraTransform());
}

Vec3 CameraBase::GetViewPos() const {
    return orientation_.position;
}

Vec3 CameraBase::GetDirection() const {
    return Vec3::Normalize(orientation_.direction);
}

Vec3 CameraBase::GetHorizon() const {
    return Vec3::Normalize(orientation_.horizon);
}

Vec3 CameraBase::GetVertical() const {
    return orientation_.horizon.VectorProd(orientation_.direction).Normalize();
}

Mat4 CameraBase::GetCameraTransform() const {
    return Mat4::Translation(orientation_.position) * GetOrientationTransform();
}

Mat4 CameraBase::GetOrientationTransform() const {
    return Mat4::Basis(orientation_.horizon, GetVertical(), orientation_.direction);
}

void CameraBase::Move(Vec3 translation) {
    orientation_.position += translation;
}

void CameraBase::Rotate(Vec3 axis, FloatType angle) {
    const auto transform = Mat4::Rotation(axis, angle);

    orientation_.direction = transform.Apply(orientation_.direction).XYZ();
    orientation_.horizon = transform.Apply(orientation_.horizon).XYZ();
}

void CameraBase::OnCameraChange(const CameraChange& cmaera_change) {
    const auto& rotate = cmaera_change.rotate;

    Rotate(GetVertical(), rotate.yaw);
    Rotate(orientation_.horizon, rotate.pitch);
    Rotate(orientation_.direction, rotate.roll);

    const auto& move = cmaera_change.move;
    Move(orientation_.direction * move.direct + orientation_.horizon * move.horizon + GetVertical() * move.vertical);

    out_transform_port_->Notify(GetCameraTransform());
}

}  // namespace detail

DirectCamera::DirectCamera(const CameraOrientation& orientation, const Settings& settings)
    : Base(orientation)
    , ndc_transform_(Mat4::BoxProjection(settings.width, settings.height, settings.depth)) {
}

Mat4 DirectCamera::GetNdcTransform() const {
    return ndc_transform_ * Mat4::Transpose(GetOrientationTransform()) * Mat4::Translation(-GetViewPos());
}

PerspectiveCamera::PerspectiveCamera(const CameraOrientation& orientation, const Settings& settings)
    : Base(orientation)
    , ndc_transform_(
          Mat4::PerspectiveProjection(settings.fov, settings.ratio, settings.min_distance, settings.max_distance)
      ) {
}

Mat4 PerspectiveCamera::GetNdcTransform() const {
    return ndc_transform_ * Mat4::Transpose(GetOrientationTransform()) * Mat4::Translation(-GetViewPos());
}

}  // namespace null_engine
