#pragma once

#include <ostream>

#include "constants.hpp"

namespace null_engine {

class Vec2 {
public:
    Vec2();
    explicit Vec2(FloatType size);
    Vec2(FloatType x, FloatType y);

    FloatType& X();
    FloatType& Y();

    FloatType GetX() const;
    FloatType GetY() const;

    Vec2& operator+=(FloatType offset);
    Vec2 operator+(FloatType offset) const;
    Vec2& operator+=(Vec2 other);
    Vec2 operator+(Vec2 other) const;

    Vec2& operator-=(FloatType offset);
    Vec2 operator-(FloatType offset) const;
    Vec2& operator-=(Vec2 other);
    Vec2 operator-(Vec2 other) const;
    Vec2 operator-() const;

    Vec2& operator*=(FloatType scale);
    Vec2 operator*(FloatType scale) const;
    Vec2& operator*=(Vec2 other);
    Vec2 operator*(Vec2 other) const;

    Vec2& operator/=(FloatType scale);
    Vec2 operator/(FloatType scale) const;
    Vec2& operator/=(Vec2 other);
    Vec2 operator/(Vec2 other) const;

    FloatType Length() const;

    [[nodiscard]] Vec2 Normalized() const;

    Vec2& Normalize();

    FloatType ScalarProd(Vec2 other) const;

    FloatType VectorProd(Vec2 other) const;

private:
    FloatType x_;
    FloatType y_;
};

Vec2 operator+(FloatType offset, const Vec2& vector);
Vec2 operator*(FloatType scale, const Vec2& vector);

std::ostream& operator<<(std::ostream& out, const Vec2& vector);

FloatType Area(Vec2 point_a, Vec2 point_b, Vec2 point_c);

}  // namespace null_engine
