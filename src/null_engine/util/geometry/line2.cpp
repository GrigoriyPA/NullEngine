#include "line2.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

Line2::Line2(Vec2 point_a, Vec2 point_b)
    : point_(point_a)
    , direction_(point_b - point_a) {
    assert(!direction_.IsZero() && "Points for line initialization should be different");

    direction_.Normalize();
}

Vec2 Line2::IntersectHorizontal(FloatType y) const {
    assert(!Equal(direction_.Y(), 0.0) && "Can not intersect two horizontal lines");

    const auto distance = (y - point_.Y()) / direction_.Y();
    return point_ + direction_ * distance;
}

Vec2 Line2::IntersectVertical(FloatType x) const {
    assert(!Equal(direction_.X(), 0.0) && "Can not intersect two vertical lines");

    const auto distance = (x - point_.X()) / direction_.X();
    return point_ + direction_ * distance;
}

}  // namespace null_engine
