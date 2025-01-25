#include "line.hpp"

#include "helpers.hpp"

namespace null_engine {

Line2::Line2(Vec2 point_a, Vec2 point_b)
    : point_(point_a)
    , direction_(point_b - point_a) {
    assert(!direction_.isZero() && "Points for line initialization should be different");

    direction_.normalize();
}

Vec2 Line2::IntersectHorizontal(FloatType y) const {
    assert(!Equal(direction_.y(), 0.0) && "Can not intersect two horizontal lines");

    const auto distance = (y - point_.y()) / direction_.y();
    return point_ + direction_ * distance;
}

Vec2 Line2::IntersectVertical(FloatType x) const {
    assert(!Equal(direction_.x(), 0.0) && "Can not intersect two vertical lines");

    const auto distance = (x - point_.x()) / direction_.x();
    return point_ + direction_ * distance;
}

}  // namespace null_engine
