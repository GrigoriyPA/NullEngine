#pragma once

#include "vector3.hpp"

namespace null_engine {

class Vec4 {
public:
    constexpr Vec4() = default;

    explicit Vec4(Vec3 xyz, FloatType h);

    constexpr Vec4(FloatType x, FloatType y, FloatType z, FloatType h)
        : x_(x)
        , y_(y)
        , z_(z)
        , h_(h) {
    }

    constexpr static Vec4 Ident(FloatType size) {
        return Vec4(size, size, size, size);
    }

    FloatType& X();
    FloatType& Y();
    FloatType& Z();
    FloatType& H();

    FloatType X() const;
    FloatType Y() const;
    FloatType Z() const;
    FloatType H() const;

    Vec2 XY() const;
    Vec3 XYZ() const;

    Vec4& operator+=(FloatType offset);
    friend Vec4 operator+(Vec4 vector, FloatType offset);
    Vec4& operator+=(Vec4 other);
    friend Vec4 operator+(Vec4 vector, Vec4 other);

    Vec4& operator-=(FloatType offset);
    friend Vec4 operator-(Vec4 vector, FloatType offset);
    Vec4& operator-=(Vec4 other);
    friend Vec4 operator-(Vec4 vector, Vec4 other);
    Vec4 operator-() const;

    Vec4& operator*=(FloatType scale);
    friend Vec4 operator*(Vec4 vector, FloatType scale);
    Vec4& operator*=(Vec4 other);
    friend Vec4 operator*(Vec4 vector, Vec4 other);

    Vec4& operator/=(FloatType scale);
    friend Vec4 operator/(Vec4 vector, FloatType scale);
    Vec4& operator/=(Vec4 other);
    friend Vec4 operator/(Vec4 vector, Vec4 other);

    bool IsZero() const;

    FloatType Length() const;

    Vec4& Normalize();
    static Vec4 Normalize(Vec4 other);

    FloatType ScalarProd(Vec4 other) const;

    Vec4& Clamp(FloatType min_value, FloatType max_value);

private:
    FloatType x_ = 0.0;
    FloatType y_ = 0.0;
    FloatType z_ = 0.0;
    FloatType h_ = 0.0;
};

Vec4 operator+(FloatType offset, Vec4 vector);
Vec4 operator*(FloatType scale, Vec4 vector);

std::ostream& operator<<(std::ostream& out, Vec4 vector);

}  // namespace null_engine
