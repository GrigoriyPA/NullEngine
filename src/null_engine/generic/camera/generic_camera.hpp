#pragma once

#include <null_engine/util/geometry/vector_3d.hpp>
#include <null_engine/util/geometry/transformation.hpp>

#include <memory>

namespace null_engine::generic {

//
// Сommon camera interface, holds information about position in scene space,
// view direction and other projection settings
//
class Camera {
public:
    using Ptr = std::shared_ptr<Camera>;

public:
    // Transform from scene space to normalized device coordinates:
    // -- NDC coordinates is box [-1, 1] for all coordinates
    virtual util::Transform GetNdcTransform() const = 0;
};

//
// Interface for camera objects which can be moved by control systems
//
class MovableCamera : public Camera {
public:
    using Ptr = std::shared_ptr<MovableCamera>;

public:
    // Camera position in scene space
    virtual util::Vec3 GetPosition() const = 0;

    // Camera right-hand normalized orientation in scene space
    virtual util::Vec3 GetDirection() const = 0;
    virtual util::Vec3 GetHorizon() const = 0;
    virtual util::Vec3 GetVertical() const = 0;

    // Move camera relative to scene coordinates
    virtual void Move(util::Vec3 translation) = 0;

    // Rotete camera relative to a camera position
    virtual void Rotate(util::Vec3 axis, util::FloatType angle) = 0;  // angle in radians
};

}  // namespace null_engine::generic
