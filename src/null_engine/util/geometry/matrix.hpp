#pragma once

#include <assimp/matrix4x4.h>

#include "vector.hpp"

namespace null_engine {

using Mat3 = Eigen::Matrix3<float>;
using Mat4 = Eigen::Matrix4<float>;

using Transform = Eigen::Transform<float, 3, Eigen::Affine>;
using ProjectiveTransform = Eigen::Transform<float, 3, Eigen::Projective>;

Transform NormalTransform(const Transform& transform);

Transform Ident();

Transform Scale(Vec3 scale);

Transform Scale(float scale_x, float scale_y, float scale_z);

Transform Scale(float scale);

Transform Translation(Vec3 translation);

Transform Translation(float translation_x, float translation_y, float translation_z);

Transform Rotation(Vec3 axis, float angle);

Transform Basis(Vec3 x, Vec3 y, Vec3 z);

ProjectiveTransform BoxProjection(float width, float height, float depth);

ProjectiveTransform PerspectiveProjection(float fov, float ratio, float min_distance, float max_distance);

ProjectiveTransform ComposeCameraTransform(
    const ProjectiveTransform& ndc_transform, const Transform& orientation_transform, Vec3 view_pos
);

}  // namespace null_engine
