#pragma once

#include "vector2.hpp"

namespace null_engine {

class Vec3 {
public:
    constexpr Vec3() = default;

    Vec3(Vec2 xy, FloatType z);

    constexpr Vec3(FloatType x, FloatType y, FloatType z)
        : x_(x)
        , y_(y)
        , z_(z) {
    }

    constexpr static Vec3 Ident(FloatType size) {
        return Vec3(size, size, size);
    }

    FloatType& X();
    FloatType& Y();
    FloatType& Z();

    FloatType X() const;
    FloatType Y() const;
    FloatType Z() const;

    Vec2 XY() const;

    Vec3& operator+=(FloatType offset);
    friend Vec3 operator+(Vec3 vector, FloatType offset);
    Vec3& operator+=(Vec3 other);
    friend Vec3 operator+(Vec3 vector, Vec3 other);

    Vec3& operator-=(FloatType offset);
    friend Vec3 operator-(Vec3 vector, FloatType offset);
    Vec3& operator-=(Vec3 other);
    friend Vec3 operator-(Vec3 vector, Vec3 other);
    Vec3 operator-() const;

    Vec3& operator*=(FloatType scale);
    friend Vec3 operator*(Vec3 vector, FloatType scale);
    Vec3& operator*=(Vec3 other);
    friend Vec3 operator*(Vec3 vector, Vec3 other);

    Vec3& operator/=(FloatType scale);
    friend Vec3 operator/(Vec3 vector, FloatType scale);
    Vec3& operator/=(Vec3 other);
    friend Vec3 operator/(Vec3 vector, Vec3 other);

    bool IsZero() const;

    FloatType Length() const;

    Vec3& Normalize();
    static Vec3 Normalize(Vec3 other);

    FloatType ScalarProd(Vec3 other) const;
    Vec3 VectorProd(Vec3 other) const;
    Vec3 Horizon() const;

    Vec3& Clamp(FloatType min_value, FloatType max_value);

private:
    FloatType x_ = 0.0;
    FloatType y_ = 0.0;
    FloatType z_ = 0.0;
};

Vec3 operator+(FloatType offset, Vec3 vector);
Vec3 operator*(FloatType scale, Vec3 vector);

std::ostream& operator<<(std::ostream& out, Vec3 vector);

}  // namespace null_engine
