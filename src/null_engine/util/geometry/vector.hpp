#pragma once

#include <Eigen/Dense>

#include "constants.hpp"

namespace null_engine {

using Vec2 = Eigen::Vector2<FloatType>;
using Vec3 = Eigen::Vector3<FloatType>;
using Vec4 = Eigen::Vector4<FloatType>;

FloatType OrientedArea(Vec2 left, Vec2 right);

FloatType OrientedArea(Vec2 point_a, Vec2 point_b, Vec2 point_c);

Vec3 VectorProd(Vec3 left, Vec3 right);

Vec3 Horizon(Vec3 vector);

}  // namespace null_engine
