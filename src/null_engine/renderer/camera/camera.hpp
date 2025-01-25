#pragma once

#include <null_engine/util/geometry/matrix.hpp>
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

class CameraBase {
public:
    explicit CameraBase(const CameraOrientation& orientation);

    InPort<CameraChange>* GetChangePort();

    void SubscribeOnCameraTransform(InPort<Transform>* observer_port) const;

    Vec3 GetViewPos() const;

    Vec3 GetDirection() const;

    Vec3 GetHorizon() const;

    Vec3 GetVertical() const;

    Transform GetCameraTransform() const;

    Transform GetOrientationTransform() const;

private:
    void Move(Vec3 translation);

    void Rotate(Vec3 axis, FloatType angle);

    void OnCameraChange(const CameraChange& cmaera_change);

    CameraOrientation orientation_;
    InPort<CameraChange> in_change_port_;
    OutPort<Transform>::Ptr out_transform_port_ = OutPort<Transform>::Make();
};

class DirectCamera : public CameraBase {
    using Base = CameraBase;

public:
    struct Settings {
        FloatType width;
        FloatType height;
        FloatType depth;
    };

    DirectCamera(const CameraOrientation& orientation, const Settings& settings);

    ProjectiveTransform GetNdcTransform() const;

private:
    ProjectiveTransform ndc_transform_;
};

class PerspectiveCamera : public CameraBase {
    using Base = CameraBase;

public:
    struct Settings {
        FloatType fov;
        FloatType ratio;
        FloatType min_distance;
        FloatType max_distance;
    };

    PerspectiveCamera(const CameraOrientation& orientation, const Settings& settings);

    ProjectiveTransform GetNdcTransform() const;

private:
    ProjectiveTransform ndc_transform_;
};

}  // namespace null_engine
