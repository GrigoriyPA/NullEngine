#pragma once

#include <assimp/vector3.h>

#include <Eigen/Dense>
#include <null_engine/acceleration/program.hpp>

#include "constants.hpp"

namespace null_engine {

using Vec2 = Eigen::Vector2<FloatType>;
using Vec3 = Eigen::Vector3<FloatType>;
using Vec4 = Eigen::Vector4<FloatType>;

FloatType OrientedArea(Vec2 left, Vec2 right);

FloatType OrientedArea(Vec2 point_a, Vec2 point_b, Vec2 point_c);

Vec3 VectorProd(Vec3 left, Vec3 right);

Vec3 Horizon(Vec3 vector);

namespace multithread::detail {

Program GetVectorFunctionsProgram();

}  // namespace multithread::detail

}  // namespace null_engine
