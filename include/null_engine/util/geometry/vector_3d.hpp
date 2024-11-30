#pragma once

#include "constants.hpp"

namespace null_engine::util {

//
// Simple 3D vector with basic operations support
//
class Vec3 {
public:
    explicit Vec3(FloatType size);
    Vec3(FloatType x, FloatType y);
    Vec3(FloatType x, FloatType y, FloatType z);

    FloatType& X();
    FloatType& Y();
    FloatType& Z();
    FloatType& H();

    FloatType GetX() const;
    FloatType GetY() const;
    FloatType GetZ() const;
    FloatType GetH() const;

    Vec3& operator+=(FloatType offset);
    Vec3 operator+(FloatType offset) const;
    Vec3& operator+=(Vec3 other);
    Vec3 operator+(Vec3 other) const;

    Vec3& operator-=(FloatType offset);
    Vec3 operator-(FloatType offset) const;
    Vec3& operator-=(Vec3 other);
    Vec3 operator-(Vec3 other) const;
    Vec3 operator-() const;

    Vec3& operator*=(FloatType scale);
    Vec3 operator*(FloatType scale) const;
    Vec3& operator*=(Vec3 other);
    Vec3 operator*(Vec3 other) const;

    Vec3& operator/=(FloatType scale);
    Vec3 operator/(FloatType scale) const;
    Vec3& operator/=(Vec3 other);
    Vec3 operator/(Vec3 other) const;

    Vec3& Clamp(FloatType min_value, FloatType max_value);

private:
    FloatType x_;
    FloatType y_;
    FloatType z_;
    FloatType h_;  // Homogeneous coordinate
};

Vec3 operator+(FloatType offset, const Vec3& vector);
Vec3 operator*(FloatType scale, const Vec3& vector);

}  // namespace null_engine::util
