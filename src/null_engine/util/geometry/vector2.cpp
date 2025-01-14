#include "vector2.hpp"

namespace null_engine {

FloatType& Vec2::X() {
    return x_;
}

FloatType& Vec2::Y() {
    return y_;
}

FloatType Vec2::GetX() const {
    return x_;
}

FloatType Vec2::GetY() const {
    return y_;
}

Vec2& Vec2::operator+=(FloatType offset) {
    *this += Vec2(offset);
    return *this;
}

Vec2 Vec2::operator+(FloatType offset) const {
    Vec2 copy(*this);
    return copy += offset;
}

Vec2& Vec2::operator+=(Vec2 other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
}

Vec2 Vec2::operator+(Vec2 other) const {
    Vec2 copy(*this);
    return copy += other;
}

Vec2& Vec2::operator-=(FloatType offset) {
    *this -= Vec2(offset);
    return *this;
}

Vec2 Vec2::operator-(FloatType offset) const {
    Vec2 copy(*this);
    return copy -= offset;
}

Vec2& Vec2::operator-=(Vec2 other) {
    x_ -= other.x_;
    y_ -= other.y_;
    return *this;
}

Vec2 Vec2::operator-(Vec2 other) const {
    Vec2 copy(*this);
    return copy -= other;
}

Vec2 Vec2::operator-() const {
    return Vec2(-x_, -y_);
}

Vec2& Vec2::operator*=(FloatType scale) {
    *this *= Vec2(scale);
    return *this;
}

Vec2 Vec2::operator*(FloatType scale) const {
    Vec2 copy(*this);
    return copy *= scale;
}

Vec2& Vec2::operator*=(Vec2 other) {
    x_ *= other.x_;
    y_ *= other.y_;
    return *this;
}

Vec2 Vec2::operator*(Vec2 other) const {
    Vec2 copy(*this);
    return copy *= other;
}

Vec2& Vec2::operator/=(FloatType scale) {
    *this /= Vec2(scale);
    return *this;
}

Vec2 Vec2::operator/(FloatType scale) const {
    Vec2 copy(*this);
    return copy /= scale;
}

Vec2& Vec2::operator/=(Vec2 other) {
    x_ /= other.x_;
    y_ /= other.y_;
    return *this;
}

Vec2 Vec2::operator/(Vec2 other) const {
    Vec2 copy(*this);
    return copy /= other;
}

FloatType Vec2::Length() const {
    return std::sqrt(x_ * x_ + y_ * y_);
}

Vec2 Vec2::Normalized() const {
    Vec2 copy(*this);
    return copy.Normalize();
}

Vec2& Vec2::Normalize() {
    return *this /= Length();
}

FloatType Vec2::ScalarProd(Vec2 other) const {
    return x_ * other.x_ + y_ * other.y_;
}

FloatType Vec2::VectorProd(Vec2 other) const {
    return y_ * other.x_ - x_ * other.y_;
}

Vec2 operator+(FloatType offset, const Vec2& vector) {
    return vector + offset;
}

Vec2 operator*(FloatType scale, const Vec2& vector) {
    return vector * scale;
}

std::ostream& operator<<(std::ostream& out, const Vec2& vector) {
    out << "(" << vector.GetX() << ", " << vector.GetY() << ")";
    return out;
}

FloatType Area(Vec2 point_a, Vec2 point_b, Vec2 point_c) {
    return std::abs((point_b - point_a).VectorProd(point_c - point_a));
}

}  // namespace null_engine
