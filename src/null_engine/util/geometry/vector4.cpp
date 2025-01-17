#include "vector4.hpp"

#include <cassert>

#include "helpers.hpp"

namespace null_engine {

Vec4::Vec4(Vec3 xyz, FloatType h)
    : x_(xyz.X())
    , y_(xyz.Y())
    , z_(xyz.Z())
    , h_(h) {
}

FloatType& Vec4::X() {
    return x_;
}

FloatType& Vec4::Y() {
    return y_;
}

FloatType& Vec4::Z() {
    return z_;
}

FloatType& Vec4::H() {
    return h_;
}

FloatType Vec4::X() const {
    return x_;
}

FloatType Vec4::Y() const {
    return y_;
}

FloatType Vec4::Z() const {
    return z_;
}

FloatType Vec4::H() const {
    return h_;
}

Vec2 Vec4::XY() const {
    return Vec2(x_, y_);
}

Vec3 Vec4::XYZ() const {
    return Vec3(x_, y_, z_);
}

Vec4& Vec4::operator+=(FloatType offset) {
    *this += Vec4::Ident(offset);
    return *this;
}

Vec4 operator+(Vec4 vector, FloatType offset) {
    vector += offset;
    return vector;
}

Vec4& Vec4::operator+=(Vec4 other) {
    x_ += other.x_;
    y_ += other.y_;
    z_ += other.z_;
    h_ += other.h_;
    return *this;
}

Vec4 operator+(Vec4 vector, Vec4 other) {
    vector += other;
    return vector;
}

Vec4& Vec4::operator-=(FloatType offset) {
    *this -= Vec4::Ident(offset);
    return *this;
}

Vec4 operator-(Vec4 vector, FloatType offset) {
    vector -= offset;
    return vector;
}

Vec4& Vec4::operator-=(Vec4 other) {
    x_ -= other.x_;
    y_ -= other.y_;
    z_ -= other.z_;
    h_ -= other.h_;
    return *this;
}

Vec4 operator-(Vec4 vector, Vec4 other) {
    vector -= other;
    return vector;
}

Vec4 Vec4::operator-() const {
    return Vec4(-x_, -y_, -z_, -h_);
}

Vec4& Vec4::operator*=(FloatType scale) {
    *this *= Vec4::Ident(scale);
    return *this;
}

Vec4 operator*(Vec4 vector, FloatType scale) {
    vector *= scale;
    return vector;
}

Vec4& Vec4::operator*=(Vec4 other) {
    x_ *= other.x_;
    y_ *= other.y_;
    z_ *= other.z_;
    h_ *= other.h_;
    return *this;
}

Vec4 operator*(Vec4 vector, Vec4 other) {
    vector *= other;
    return vector;
}

Vec4& Vec4::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    *this /= Vec4::Ident(scale);
    return *this;
}

Vec4 operator/(Vec4 vector, FloatType scale) {
    vector /= scale;
    return vector;
}

Vec4& Vec4::operator/=(Vec4 other) {
    assert(
        !Equal(other.x_, 0.0) && !Equal(other.y_, 0.0) && !Equal(other.z_, 0.0) && !Equal(other.h_, 0.0) &&
        "Division by zero"
    );

    x_ /= other.x_;
    y_ /= other.y_;
    z_ /= other.z_;
    h_ /= other.h_;
    return *this;
}

Vec4 operator/(Vec4 vector, Vec4 other) {
    vector /= other;
    return vector;
}

bool Vec4::IsZero() const {
    return Equal(x_, 0.0) && Equal(y_, 0.0) && Equal(z_, 0.0) && Equal(h_, 0.0);
}

FloatType Vec4::Length() const {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_ + h_ * h_);
}

Vec4& Vec4::Normalize() {
    return *this /= Length();
}

Vec4 Vec4::Normalize(Vec4 other) {
    other.Normalize();
    return other;
}

FloatType Vec4::ScalarProd(Vec4 other) const {
    return x_ * other.x_ + y_ * other.y_ + z_ * other.z_ + h_ * other.h_;
}

Vec4& Vec4::Clamp(FloatType min_value, FloatType max_value) {
    assert(min_value <= max_value && "Invalid clamp parameters");

    x_ = std::min(max_value, std::max(min_value, x_));
    y_ = std::min(max_value, std::max(min_value, y_));
    z_ = std::min(max_value, std::max(min_value, z_));
    h_ = std::min(max_value, std::max(min_value, h_));
    return *this;
}

Vec4 operator+(FloatType offset, Vec4 vector) {
    return vector + offset;
}

Vec4 operator*(FloatType scale, Vec4 vector) {
    return vector * scale;
}

std::ostream& operator<<(std::ostream& out, Vec4 vector) {
    out << "(" << vector.X() << ", " << vector.Y() << ", " << vector.Z() << ", " << vector.H() << ")";
    return out;
}

}  // namespace null_engine
