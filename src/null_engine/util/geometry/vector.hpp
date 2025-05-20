#pragma once

#include <assimp/vector3.h>

#include <Eigen/Dense>
#include <null_engine/acceleration/program.hpp>

namespace null_engine {

using Vec2 = Eigen::Vector2<float>;
using Vec3 = Eigen::Vector3<float>;
using Vec4 = Eigen::Vector4<float>;

float OrientedArea(Vec2 left, Vec2 right);

float OrientedArea(Vec2 point_a, Vec2 point_b, Vec2 point_c);

Vec3 VectorProd(Vec3 left, Vec3 right);

Vec3 Horizon(Vec3 vector);

Vec3 Vec4ToVec3(Vec4 vector);

Vec4 Vec3ToVec4(Vec3 vector, float w);

namespace multithread::detail {

Program GetVectorFunctionsProgram();

}  // namespace multithread::detail

}  // namespace null_engine
