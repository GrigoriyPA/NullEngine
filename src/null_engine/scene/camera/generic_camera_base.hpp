#pragma once

#include <null_engine/util/geometry/transformation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine {

class CameraBase {
public:
    CameraBase();

    Vec3 GetPosition() const;

    Vec3 GetDirection() const;

    Vec3 GetHorizon() const;

    Vec3 GetVertical() const;

    Transform GetCameraTransform() const;

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

}  // namespace null_engine
