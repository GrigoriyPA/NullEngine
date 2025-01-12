#pragma once

#include <null_engine/util/geometry/transformation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::generic {

class CameraBase {
public:
    CameraBase();

    util::Vec3 GetPosition() const;

    util::Vec3 GetDirection() const;

    util::Vec3 GetHorizon() const;

    util::Vec3 GetVertical() const;

    // Transform from scene space to camera space
    util::Transform GetCameraTransform() const;

    CameraBase& SetPosition(util::Vec3 position);

    CameraBase& SetDirection(util::Vec3 direction);

    CameraBase& SetOrientation(util::Vec3 direction, util::Vec3 horizon);

    void Move(util::Vec3 translation);

    void Rotate(util::Vec3 axis, util::FloatType angle);

private:
    util::Vec3 position_;
    util::Vec3 direction_;
    util::Vec3 horizon_;
};

}  // namespace null_engine::generic
