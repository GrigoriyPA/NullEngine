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

    Mat4 GetCameraMat4() const;

    CameraBase& SetPosition(Vec3 position);

    CameraBase& SetDirection(Vec3 direction);

    CameraBase& SetOrientation(Vec3 direction, Vec3 horizon);

    void Move(Vec3 translation);

    void Rotate(Vec3 axis, FloatType angle);

private:
    Vec3 position_;
    Vec3 direction_;
    Vec3 horizon_;
};

}  // namespace detail

class DirectCamera : public detail::CameraBase {
public:
    DirectCamera(FloatType width, FloatType height, FloatType depth);

    Mat4 GetNdcMat4() const;

private:
    const Mat4 ndc_transform_;
};

class PerspectiveCamera : public detail::CameraBase {
public:
    PerspectiveCamera(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance);

    Mat4 GetNdcMat4() const;

private:
    const Mat4 ndc_transform_;
};

}  // namespace null_engine
