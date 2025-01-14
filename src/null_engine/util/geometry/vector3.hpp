#pragma once

#include "vector2.hpp"

namespace null_engine {

class Vec3 {
public:
    constexpr Vec3()
        : x_(0.0)
        , y_(0.0)
        , z_(0.0)
        , h_(1.0) {
    }

    constexpr explicit Vec3(FloatType size)
        : x_(size)
        , y_(size)
        , z_(size)
        , h_(1.0) {
    }

    constexpr Vec3(FloatType x, FloatType y)
        : x_(x)
        , y_(y)
        , z_(0.0)
        , h_(1.0) {
    }

    constexpr Vec3(FloatType x, FloatType y, FloatType z)
        : x_(x)
        , y_(y)
        , z_(z)
        , h_(1.0) {
    }

    constexpr Vec3(FloatType x, FloatType y, FloatType z, FloatType h)
        : x_(x)
        , y_(y)
        , z_(z)
        , h_(h) {
    }

    FloatType& X();
    FloatType& Y();
    FloatType& Z();
    FloatType& H();

    FloatType GetX() const;
    FloatType GetY() const;
    FloatType GetZ() const;
    FloatType GetH() const;

    Vec2 GetXY() const;
    Vec2 GetXZ() const;
    Vec2 GetYZ() const;

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

    FloatType ScalarProd(Vec3 other) const;

    Vec3 VectorProd(Vec3 other) const;

    Vec3 Horizon() const;

    Vec3& Clamp(FloatType min_value, FloatType max_value);

private:
    FloatType x_;
    FloatType y_;
    FloatType z_;
    FloatType h_;
};

Vec3 operator+(FloatType offset, const Vec3& vector);
Vec3 operator*(FloatType scale, const Vec3& vector);

std::ostream& operator<<(std::ostream& out, const Vec3& vector);

}  // namespace null_engine
