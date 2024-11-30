#pragma once

#include "generic_camera.hpp"

#include <null_engine/util/geometry/transformation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::generic {

//
// Base camera class which holds position and orientation.
// In initial state:
// -> position = (0, 0, 0)
// -> direction = (0, 0, 1)
// -> horizon = (1, 0, 0)
//
class CameraBase : public Camera {
public:
    CameraBase();

    CameraBase& SetPosition(util::Vec3 position);
    CameraBase& SetDirection(util::Vec3 direction);

    // Direction and horizon should be orthogonal
    CameraBase& SetOrientation(util::Vec3 direction, util::Vec3 horizon);

public:
    void Move(util::Vec3 translation);
    void Rotate(util::Vec3 axis, util::FloatType angle);  // angle in radians

private:
    void ResetCameraTransform();

protected:
    util::Vec3 position_;  // Position in scene space
    util::Vec3 direction_;
    util::Vec3 horizon_;
    util::Transform camera_transform_;  // Transform from scene space to camera space
};

}  // namespace null_engine::generic
