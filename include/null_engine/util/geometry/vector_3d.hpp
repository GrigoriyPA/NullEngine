#pragma once

#include "constants.hpp"

#include <ostream>

namespace null_engine::util {

//
// Simple 3D vector with basic operations support
//
class Vec3 {
public:
    Vec3();  // Zero vector
    explicit Vec3(FloatType size);
    Vec3(FloatType x, FloatType y);
    Vec3(FloatType x, FloatType y, FloatType z);
    Vec3(FloatType x, FloatType y, FloatType z, FloatType h);

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

    FloatType Length() const;
    [[nodiscard]] Vec3 Normalized() const;
    Vec3& Normalize();

    FloatType ScalarProd(Vec3 other) const;  // Scalar production between vectors
    Vec3 VectorProd(Vec3 other) const;       // Right hand vector production
    Vec3 Horizon() const;                    // Right hand orthogonal vector with zero Y

    Vec3& Clamp(FloatType min_value, FloatType max_value);

private:
    FloatType x_;
    FloatType y_;
    FloatType z_;
    FloatType h_;  // Homogeneous coordinate
};

Vec3 operator+(FloatType offset, const Vec3& vector);
Vec3 operator*(FloatType scale, const Vec3& vector);

std::ostream& operator<<(std::ostream& out, const Vec3& vector);

}  // namespace null_engine::util
