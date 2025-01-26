#include "camera.hpp"

namespace null_engine {

namespace {

ProjectiveTransform ComposeCameraTransform(
    const ProjectiveTransform& ndc_transform, const Transform& orientation_transform, Vec3 view_pos
) {
    return ndc_transform * Transform(orientation_transform.matrix().transpose()) * Translation(-view_pos);
}

}  // anonymous namespace

CameraBase::CameraBase(const CameraOrientation& orientation)
    : orientation_(orientation)
    , in_change_port_(std::bind(&CameraBase::OnCameraChange, this, std::placeholders::_1)) {
}

InPort<CameraChange>* CameraBase::GetChangePort() {
    return &in_change_port_;
}

void CameraBase::SubscribeOnCameraTransform(InPort<Transform>* observer_port) const {
    out_transform_port_->Subscribe(observer_port, GetCameraTransform());
}

Vec3 CameraBase::GetViewPos() const {
    return orientation_.position;
}

Vec3 CameraBase::GetDirection() const {
    return orientation_.direction.normalized();
}

Vec3 CameraBase::GetHorizon() const {
    return orientation_.horizon.normalized();
}

Vec3 CameraBase::GetVertical() const {
    return VectorProd(orientation_.horizon, orientation_.direction).normalized();
}

Transform CameraBase::GetCameraTransform() const {
    return Translation(orientation_.position) * GetOrientationTransform();
}

Transform CameraBase::GetOrientationTransform() const {
    return Basis(orientation_.horizon, GetVertical(), orientation_.direction);
}

void CameraBase::Move(Vec3 translation) {
    orientation_.position += translation;
}

void CameraBase::Rotate(Vec3 axis, FloatType angle) {
    const auto transform = Rotation(axis, angle).linear();

    orientation_.direction = transform * orientation_.direction;
    orientation_.horizon = transform * orientation_.horizon;
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

DirectCamera::DirectCamera(const CameraOrientation& orientation, const Settings& settings)
    : Base(orientation)
    , ndc_transform_(BoxProjection(settings.width, settings.height, settings.depth)) {
}

ProjectiveTransform DirectCamera::GetNdcTransform() const {
    return ComposeCameraTransform(ndc_transform_, GetOrientationTransform(), GetViewPos());
}

PerspectiveCamera::PerspectiveCamera(const CameraOrientation& orientation, const Settings& settings)
    : Base(orientation)
    , ndc_transform_(PerspectiveProjection(settings.fov, settings.ratio, settings.min_distance, settings.max_distance)
      ) {
}

ProjectiveTransform PerspectiveCamera::GetNdcTransform() const {
    return ComposeCameraTransform(ndc_transform_, GetOrientationTransform(), GetViewPos());
}

}  // namespace null_engine
