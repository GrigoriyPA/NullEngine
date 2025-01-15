#include "vector3.hpp"

#include <cassert>

#include "helpers.hpp"

namespace null_engine {

Vec3::Vec3(Vec2 xy, FloatType z)
    : x_(xy.X())
    , y_(xy.Y())
    , z_(z) {
}

FloatType& Vec3::X() {
    return x_;
}

FloatType& Vec3::Y() {
    return y_;
}

FloatType& Vec3::Z() {
    return z_;
}

FloatType& Vec3::H() {
    return h_;
}

FloatType Vec3::X() const {
    return x_;
}

FloatType Vec3::Y() const {
    return y_;
}

FloatType Vec3::Z() const {
    return z_;
}

FloatType Vec3::H() const {
    return h_;
}

Vec2 Vec3::XY() const {
    return Vec2(x_, y_);
}

Vec2 Vec3::XZ() const {
    return Vec2(x_, z_);
}

Vec2 Vec3::YZ() const {
    return Vec2(y_, z_);
}

Vec3& Vec3::operator+=(FloatType offset) {
    *this += Vec3::Ident(offset);
    return *this;
}

Vec3 operator+(Vec3 vector, FloatType offset) {
    vector += offset;
    return vector;
}

Vec3& Vec3::operator+=(Vec3 other) {
    x_ += other.x_;
    y_ += other.y_;
    z_ += other.z_;
    return *this;
}

Vec3 operator+(Vec3 vector, Vec3 other) {
    vector += other;
    return vector;
}

Vec3& Vec3::operator-=(FloatType offset) {
    *this -= Vec3::Ident(offset);
    return *this;
}

Vec3 operator-(Vec3 vector, FloatType offset) {
    vector -= offset;
    return vector;
}

Vec3& Vec3::operator-=(Vec3 other) {
    x_ -= other.x_;
    y_ -= other.y_;
    z_ -= other.z_;
    return *this;
}

Vec3 operator-(Vec3 vector, Vec3 other) {
    vector -= other;
    return vector;
}

Vec3 Vec3::operator-() const {
    return Vec3(-x_, -y_, -z_, h_);
}

Vec3& Vec3::operator*=(FloatType scale) {
    *this *= Vec3::Ident(scale);
    return *this;
}

Vec3 operator*(Vec3 vector, FloatType scale) {
    vector *= scale;
    return vector;
}

Vec3& Vec3::operator*=(Vec3 other) {
    x_ *= other.x_;
    y_ *= other.y_;
    z_ *= other.z_;
    return *this;
}

Vec3 operator*(Vec3 vector, Vec3 other) {
    vector *= other;
    return vector;
}

Vec3& Vec3::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    *this /= Vec3::Ident(scale);
    return *this;
}

Vec3 operator/(Vec3 vector, FloatType scale) {
    vector /= scale;
    return vector;
}

Vec3& Vec3::operator/=(Vec3 other) {
    assert(!Equal(other.x_, 0.0) && !Equal(other.y_, 0.0) && !Equal(other.z_, 0.0) && "Division by zero");

    x_ /= other.x_;
    y_ /= other.y_;
    z_ /= other.z_;
    return *this;
}

Vec3 operator/(Vec3 vector, Vec3 other) {
    vector /= other;
    return vector;
}

bool Vec3::IsZero() const {
    return Equal(x_, 0.0) && Equal(y_, 0.0) && Equal(z_, 0.0);
}

FloatType Vec3::Length() const {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

Vec3& Vec3::Normalize() {
    return *this /= Length();
}

Vec3 Vec3::Normalize(Vec3 other) {
    other.Normalize();
    return other;
}

FloatType Vec3::ScalarProd(Vec3 other) const {
    return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
}

Vec3 Vec3::VectorProd(Vec3 other) const {
    return Vec3(z_ * other.y_ - y_ * other.z_, x_ * other.z_ - z_ * other.x_, y_ * other.x_ - x_ * other.y_);
}

Vec3 Vec3::Horizon() const {
    if (Equal(x_, 0.0) && Equal(z_, 0.0)) {
        return Vec3(1.0, 0.0, 0.0);
    }

    return Vec3(z_, 0.0, -x_);
}

Vec3& Vec3::Clamp(FloatType min_value, FloatType max_value) {
    assert(min_value <= max_value && "Invalid clamp parameters");

    x_ = std::min(max_value, std::max(min_value, x_));
    y_ = std::min(max_value, std::max(min_value, y_));
    z_ = std::min(max_value, std::max(min_value, z_));
    return *this;
}

Vec3 operator+(FloatType offset, Vec3 vector) {
    return vector + offset;
}

Vec3 operator*(FloatType scale, Vec3 vector) {
    return vector * scale;
}

std::ostream& operator<<(std::ostream& out, Vec3 vector) {
    out << "(" << vector.X() << ", " << vector.Y() << ", " << vector.Z() << ") [" << vector.H() << "]";
    return out;
}

}  // namespace null_engine
