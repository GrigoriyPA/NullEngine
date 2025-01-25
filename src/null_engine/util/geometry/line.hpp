#pragma once

#include "vector.hpp"

namespace null_engine {

class Line2 {
public:
    Line2(Vec2 point_a, Vec2 point_b);

    Vec2 IntersectHorizontal(FloatType y) const;

    Vec2 IntersectVertical(FloatType x) const;

private:
    Vec2 point_;
    Vec2 direction_;
};

}  // namespace null_engine
