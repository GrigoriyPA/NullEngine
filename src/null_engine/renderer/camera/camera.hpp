#pragma once

#include <null_engine/util/geometry/matrix4.hpp>

namespace null_engine {

namespace detail {

class CameraBase {
public:
    CameraBase();

    Vec3 GetPosition() const;

    Vec3 GetDirection() const;

    Vec3 GetHorizon() const;

    Vec3 GetVertical() const;

    Mat4 GetCameraTransform() const;

    CameraBase& SetPosition(Vec3 position);

    CameraBase& SetDirection(Vec3 direction);

    CameraBase& SetOrientation(Vec3 direction, Vec3 horizon);

    void MoveGlobal(Vec3 translation);

    void Move(FloatType direct_move, FloatType horizon_move, FloatType vertical_move);

    void RotateGlobal(Vec3 axis, FloatType angle);

    void Rotate(FloatType yaw_rotation, FloatType pitch_rotation, FloatType roll_rotation);

private:
    Vec3 position_;
    Vec3 direction_;
    Vec3 horizon_;
};

}  // namespace detail

class DirectCamera : public detail::CameraBase {
public:
    DirectCamera(FloatType width, FloatType height, FloatType depth);

    Mat4 GetNdcTransform() const;

private:
    const Mat4 ndc_transform_;
};

class PerspectiveCamera : public detail::CameraBase {
public:
    PerspectiveCamera(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance);

    Mat4 GetNdcTransform() const;

private:
    const Mat4 ndc_transform_;
};

}  // namespace null_engine
