#pragma once

#include "vector.hpp"

namespace null_engine {

using Mat3 = Eigen::Matrix3<FloatType>;
using Mat4 = Eigen::Matrix4<FloatType>;

using Transform = Eigen::Transform<FloatType, 3, Eigen::Affine>;
using ProjectiveTransform = Eigen::Transform<FloatType, 3, Eigen::Projective>;

Transform NormalTransform(const Transform& transform);

Transform Ident();

Transform Scale(Vec3 scale);

Transform Scale(FloatType scale_x, FloatType scale_y, FloatType scale_z);

Transform Scale(FloatType scale);

Transform Translation(Vec3 translation);

Transform Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z);

Transform Rotation(Vec3 axis, FloatType angle);

Transform Basis(Vec3 x, Vec3 y, Vec3 z);

ProjectiveTransform BoxProjection(FloatType width, FloatType height, FloatType depth);

ProjectiveTransform PerspectiveProjection(
    FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance
);

}  // namespace null_engine
