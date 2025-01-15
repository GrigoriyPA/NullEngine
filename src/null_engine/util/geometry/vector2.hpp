#pragma once

#include <ostream>

#include "constants.hpp"

namespace null_engine {

class Vec2 {
public:
    constexpr Vec2() = default;

    constexpr Vec2(FloatType x, FloatType y)
        : x_(x)
        , y_(y) {
    }

    constexpr static Vec2 Ident(FloatType size) {
        return Vec2(size, size);
    }

    FloatType& X();
    FloatType& Y();

    FloatType X() const;
    FloatType Y() const;

    Vec2& operator+=(FloatType offset);
    friend Vec2 operator+(Vec2 vector, FloatType offset);
    Vec2& operator+=(Vec2 other);
    friend Vec2 operator+(Vec2 vector, Vec2 other);

    Vec2& operator-=(FloatType offset);
    friend Vec2 operator-(Vec2 vector, FloatType offset);
    Vec2& operator-=(Vec2 other);
    friend Vec2 operator-(Vec2 vector, Vec2 other);
    Vec2 operator-() const;

    Vec2& operator*=(FloatType scale);
    friend Vec2 operator*(Vec2 vector, FloatType scale);
    Vec2& operator*=(Vec2 other);
    friend Vec2 operator*(Vec2 vector, Vec2 other);

    Vec2& operator/=(FloatType scale);
    friend Vec2 operator/(Vec2 vector, FloatType scale);
    Vec2& operator/=(Vec2 other);
    friend Vec2 operator/(Vec2 vector, Vec2 other);

    bool IsZero() const;

    FloatType Length() const;

    Vec2& Normalize();
    static Vec2 Normalize(Vec2 other);

    FloatType ScalarProd(Vec2 other) const;
    FloatType VectorProd(Vec2 other) const;

    Vec2& Clamp(FloatType min_value, FloatType max_value);

private:
    FloatType x_ = 0.0;
    FloatType y_ = 0.0;
};

Vec2 operator+(FloatType offset, Vec2 vector);
Vec2 operator*(FloatType scale, Vec2 vector);

std::ostream& operator<<(std::ostream& out, Vec2 vector);

FloatType OrientedArea(Vec2 point_a, Vec2 point_b, Vec2 point_c);

}  // namespace null_engine
