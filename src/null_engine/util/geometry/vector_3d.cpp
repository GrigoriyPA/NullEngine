#include <null_engine/util/geometry/vector_3d.hpp>

#include <algorithm>

namespace null_engine::util {

//// Vec3

Vec3::Vec3(FloatType size)
    : x_(size)
    , y_(size)
    , z_(size)
    , h_(1.0) {
}

Vec3::Vec3(FloatType x, FloatType y)
    : x_(x)
    , y_(y)
    , z_(0.0)
    , h_(1.0) {
}

Vec3::Vec3(FloatType x, FloatType y, FloatType z)
    : x_(x)
    , y_(y)
    , z_(z)
    , h_(1.0) {
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

FloatType Vec3::GetX() const {
    return x_;
}

FloatType Vec3::GetY() const {
    return y_;
}

FloatType Vec3::GetZ() const {
    return z_;
}

FloatType Vec3::GetH() const {
    return h_;
}

Vec3& Vec3::operator+=(FloatType offset) {
    *this += Vec3(offset);
    return *this;
}

Vec3 Vec3::operator+(FloatType offset) const {
    Vec3 copy(*this);
    return copy += offset;
}

Vec3& Vec3::operator+=(Vec3 other) {
    x_ += other.x_;
    y_ += other.y_;
    z_ += other.z_;
    return *this;
}

Vec3 Vec3::operator+(Vec3 other) const {
    Vec3 copy(*this);
    return copy += other;
}

Vec3& Vec3::operator-=(FloatType offset) {
    *this -= Vec3(offset);
    return *this;
}

Vec3 Vec3::operator-(FloatType offset) const {
    Vec3 copy(*this);
    return copy -= offset;
}

Vec3& Vec3::operator-=(Vec3 other) {
    x_ -= other.x_;
    y_ -= other.y_;
    z_ -= other.z_;
    return *this;
}

Vec3 Vec3::operator-(Vec3 other) const {
    Vec3 copy(*this);
    return copy -= other;
}

Vec3 Vec3::operator-() const {
    return Vec3(-x_, -y_, -z_);
}

Vec3& Vec3::operator*=(FloatType scale) {
    *this *= Vec3(scale);
    return *this;
}

Vec3 Vec3::operator*(FloatType scale) const {
    Vec3 copy(*this);
    return copy *= scale;
}

Vec3& Vec3::operator*=(Vec3 other) {
    x_ *= other.x_;
    y_ *= other.y_;
    z_ *= other.z_;
    return *this;
}

Vec3 Vec3::operator*(Vec3 other) const {
    Vec3 copy(*this);
    return copy *= other;
}

Vec3& Vec3::operator/=(FloatType scale) {
    *this /= Vec3(scale);
    return *this;
}

Vec3 Vec3::operator/(FloatType scale) const {
    Vec3 copy(*this);
    return copy /= scale;
}

Vec3& Vec3::operator/=(Vec3 other) {
    x_ /= other.x_;
    y_ /= other.y_;
    z_ /= other.z_;
    return *this;
}

Vec3 Vec3::operator/(Vec3 other) const {
    Vec3 copy(*this);
    return copy /= other;
}

Vec3& Vec3::Clamp(FloatType min_value, FloatType max_value) {
    x_ = std::min(max_value, std::max(min_value, x_));
    y_ = std::min(max_value, std::max(min_value, y_));
    z_ = std::min(max_value, std::max(min_value, z_));
    return *this;
}

//// Vec3 external operators

Vec3 operator+(FloatType offset, const Vec3& vector) {
    return vector + offset;
}

Vec3 operator*(FloatType scale, const Vec3& vector) {
    return vector * scale;
}

}  // namespace null_engine::util
