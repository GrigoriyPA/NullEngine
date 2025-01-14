#include "vector2.hpp"

#include <cassert>

#include "helpers.hpp"

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
    *this += Vec2::Ident(offset);
    return *this;
}

Vec2 operator+(Vec2 vector, FloatType offset) {
    vector += offset;
    return vector;
}

Vec2& Vec2::operator+=(Vec2 other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
}

Vec2 operator+(Vec2 vector, Vec2 other) {
    vector += other;
    return vector;
}

Vec2& Vec2::operator-=(FloatType offset) {
    *this -= Vec2::Ident(offset);
    return *this;
}

Vec2 operator-(Vec2 vector, FloatType offset) {
    vector -= offset;
    return vector;
}

Vec2& Vec2::operator-=(Vec2 other) {
    x_ -= other.x_;
    y_ -= other.y_;
    return *this;
}

Vec2 operator-(Vec2 vector, Vec2 other) {
    vector -= other;
    return vector;
}

Vec2 Vec2::operator-() const {
    return Vec2(-x_, -y_);
}

Vec2& Vec2::operator*=(FloatType scale) {
    *this *= Vec2::Ident(scale);
    return *this;
}

Vec2 operator*(Vec2 vector, FloatType scale) {
    vector *= scale;
    return vector;
}

Vec2& Vec2::operator*=(Vec2 other) {
    x_ *= other.x_;
    y_ *= other.y_;
    return *this;
}

Vec2 operator*(Vec2 vector, Vec2 other) {
    vector *= other;
    return vector;
}

Vec2& Vec2::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    *this /= Vec2::Ident(scale);
    return *this;
}

Vec2 operator/(Vec2 vector, FloatType scale) {
    vector /= scale;
    return vector;
}

Vec2& Vec2::operator/=(Vec2 other) {
    assert(!Equal(other.x_, 0.0) && !Equal(other.y_, 0.0) && "Division by zero");

    x_ /= other.x_;
    y_ /= other.y_;
    return *this;
}

Vec2 operator/(Vec2 vector, Vec2 other) {
    vector /= other;
    return vector;
}

FloatType Vec2::Length() const {
    return std::sqrt(x_ * x_ + y_ * y_);
}

Vec2& Vec2::Normalize() {
    return *this /= Length();
}

Vec2 Vec2::Normalize(Vec2 other) {
    other.Normalize();
    return other;
}

FloatType Vec2::ScalarProd(Vec2 other) const {
    return x_ * other.x_ + y_ * other.y_;
}

FloatType Vec2::VectorProd(Vec2 other) const {
    return y_ * other.x_ - x_ * other.y_;
}

Vec2 operator+(FloatType offset, Vec2 vector) {
    return vector + offset;
}

Vec2 operator*(FloatType scale, Vec2 vector) {
    return vector * scale;
}

std::ostream& operator<<(std::ostream& out, Vec2 vector) {
    out << "(" << vector.GetX() << ", " << vector.GetY() << ")";
    return out;
}

FloatType Area(Vec2 point_a, Vec2 point_b, Vec2 point_c) {
    return std::abs((point_b - point_a).VectorProd(point_c - point_a));
}

}  // namespace null_engine
