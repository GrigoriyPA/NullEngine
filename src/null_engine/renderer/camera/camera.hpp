#pragma once

#include <null_engine/util/geometry/matrix4.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

struct CameraChange {
    struct Move {
        FloatType direct = 0.0;
        FloatType horizon = 0.0;
        FloatType vertical = 0.0;
    };

    struct Rotation {
        FloatType yaw = 0.0;
        FloatType pitch = 0.0;
        FloatType roll = 0.0;
    };

    Move move;
    Rotation rotate;
};

struct CameraOrientation {
    Vec3 position;
    Vec3 direction;
    Vec3 horizon;
};

namespace detail {

class CameraBase {
public:
    explicit CameraBase(const CameraOrientation& orientation);

    InPort<CameraChange>* GetChangePort();

    void SubscribeOnCameraTransform(InPort<Mat4>* observer_port) const;

    Vec3 GetViewPos() const;

    Vec3 GetDirection() const;

    Vec3 GetHorizon() const;

    Vec3 GetVertical() const;

    Mat4 GetCameraTransform() const;

    Mat4 GetOrientationTransform() const;

private:
    void Move(Vec3 translation);

    void Rotate(Vec3 axis, FloatType angle);

    void OnCameraChange(const CameraChange& cmaera_change);

    CameraOrientation orientation_;
    InPort<CameraChange> in_change_port_;
    OutPort<Mat4>::Ptr out_transform_port_ = OutPort<Mat4>::Make();
};

}  // namespace detail

class DirectCamera : public detail::CameraBase {
    using Base = detail::CameraBase;

public:
    struct Settings {
        FloatType width;
        FloatType height;
        FloatType depth;
    };

    DirectCamera(const CameraOrientation& orientation, const Settings& settings);

    Mat4 GetNdcTransform() const;

private:
    const Mat4 ndc_transform_;
};

class PerspectiveCamera : public detail::CameraBase {
    using Base = detail::CameraBase;

public:
    struct Settings {
        FloatType fov;
        FloatType ratio;
        FloatType min_distance;
        FloatType max_distance;
    };

    PerspectiveCamera(const CameraOrientation& orientation, const Settings& settings);

    Mat4 GetNdcTransform() const;

private:
    const Mat4 ndc_transform_;
};

}  // namespace null_engine
