#include "interpolation.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine::detail {

Interpolation::Interpolation(const Vertex& point)
    : z_(point.position.Z())
    , h_(point.position.H())
    , params_(point.params) {
    params_ *= h_;
}

FloatType Interpolation::GetZ() const {
    return z_;
}

FloatType Interpolation::GetH() const {
    return h_;
}

const VertexParams& Interpolation::GetParams() const {
    return params_;
}

Interpolation& Interpolation::operator+=(const Interpolation& other) {
    z_ += other.z_;
    h_ += other.h_;
    params_ += other.params_;
    return *this;
}

Interpolation operator+(Interpolation left, const Interpolation& right) {
    left += right;
    return left;
}

Interpolation& Interpolation::operator-=(const Interpolation& other) {
    z_ -= other.z_;
    h_ -= other.h_;
    params_ -= other.params_;
    return *this;
}

Interpolation operator-(Interpolation left, const Interpolation& right) {
    left -= right;
    return left;
}

Interpolation& Interpolation::operator*=(FloatType scale) {
    z_ *= scale;
    h_ *= scale;
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
    h_ /= scale;
    params_ /= scale;
    return *this;
}

Interpolation operator/(Interpolation left, FloatType scale) {
    left /= scale;
    return left;
}

PixelCounted::PixelCounted(uint64_t number_pixels)
    : number_pixels_(number_pixels) {
}

bool PixelCounted::HasPixels() const {
    return number_pixels_ > 0;
}

void PixelCounted::DecreasePixels() {
    assert(number_pixels_ && "Can not increment after finish");
    --number_pixels_;
}

TriangleBorders::TriangleBorders(DirValue<int64_t> y, const Border& left, const Border& right, uint64_t number_pixels)
    : PixelCounted(number_pixels)
    , y_(y)
    , left_(left)
    , right_(right) {
}

int64_t TriangleBorders::GetY() const {
    return y_.Get();
}

FloatType TriangleBorders::GetLeftBorder() const {
    return left_.border.Get();
}

FloatType TriangleBorders::GetRightBorder() const {
    return right_.border.Get();
}

const Interpolation& TriangleBorders::GetLeftInterpolation() const {
    return left_.interpolation.Get();
}

const Interpolation& TriangleBorders::GetRightInterpolation() const {
    return right_.interpolation.Get();
}

void TriangleBorders::Increment() {
    DecreasePixels();

    y_.Increment();
    left_.border.Increment();
    left_.interpolation.Increment();
    right_.border.Increment();
    right_.interpolation.Increment();
}

RsteriztionLine::RsteriztionLine(
    DirValue<int64_t> x, int64_t y, const DirValue<Interpolation>& interpolation, uint64_t number_pixels
)
    : PixelCounted(number_pixels)
    , x_(x)
    , y_(y)
    , interpolation_(interpolation) {
}

int64_t RsteriztionLine::GetX() const {
    return x_.Get();
}

int64_t RsteriztionLine::GetY() const {
    return y_;
}

const Interpolation& RsteriztionLine::GetInterpolation() const {
    return interpolation_.Get();
}

void RsteriztionLine::Increment() {
    DecreasePixels();

    x_.Increment();
    interpolation_.Increment();
}

}  // namespace null_engine::detail
