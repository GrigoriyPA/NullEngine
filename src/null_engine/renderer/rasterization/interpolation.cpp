#include "interpolation.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine::native::detail {

Interpolation::Interpolation(FloatType z, FloatType w, const InterpolationParams& params)
    : z_(z)
    , w_(w)
    , params_(params) {
    params_ *= w_;
}

FloatType Interpolation::GetZ() const {
    return z_;
}

Interpolation::InterpolationParams Interpolation::GetParams() const {
    InterpolationParams result = params_;
    result /= w_;
    return result;
}

Interpolation& Interpolation::operator+=(const Interpolation& other) {
    z_ += other.z_;
    w_ += other.w_;
    params_ += other.params_;
    return *this;
}

Interpolation operator+(Interpolation left, const Interpolation& right) {
    left += right;
    return left;
}

Interpolation& Interpolation::operator-=(const Interpolation& other) {
    z_ -= other.z_;
    w_ -= other.w_;
    params_ -= other.params_;
    return *this;
}

Interpolation operator-(Interpolation left, const Interpolation& right) {
    left -= right;
    return left;
}

Interpolation& Interpolation::operator*=(FloatType scale) {
    z_ *= scale;
    w_ *= scale;
    params_ *= scale;
    return *this;
}

Interpolation operator*(Interpolation left, FloatType scale) {
    left *= scale;
    return left;
}

Interpolation& Interpolation::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    z_ /= scale;
    w_ /= scale;
    params_ /= scale;
    return *this;
}

Interpolation operator/(Interpolation left, FloatType scale) {
    left /= scale;
    return left;
}

HorizontalLine::HorizontalLine(const VertexInfo& vertex_a, const VertexInfo& vertex_b)
    : x_(vertex_a.x, vertex_a.x <= vertex_b.x ? 1 : -1)
    , y_(vertex_a.y)
    , number_pixels_(std::abs(vertex_a.x - vertex_b.x) + 1)
    , interpolation_(vertex_a.interpolation, vertex_b.interpolation, number_pixels_) {
    assert(vertex_a.y == vertex_b.y && "Expected horizontal line");
}

bool HorizontalLine::Finished() const {
    return number_pixels_ == 0;
}

VertexInfo HorizontalLine::GetVertex() const {
    return {.x = x_.Get(), .y = y_, .interpolation = interpolation_.Get()};
}

void HorizontalLine::Increment() {
    assert(!Finished() && "Can not increment after finish");

    x_.Increment();
    interpolation_.Increment();
    --number_pixels_;
}

}  // namespace null_engine::native::detail
