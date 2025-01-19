#include "helpers.hpp"

#include <cassert>
#include <cmath>

namespace null_engine {

bool Equal(FloatType left, FloatType right) {
    return std::abs(right - left) <= kEps;
}

bool Less(FloatType left, FloatType right) {
    return left <= right && !Equal(left, right);
}

FloatType Module(FloatType left, FloatType right) {
    assert(right > kEps && "Division by zero");

    const int64_t delta = std::floor(left / right);
    left -= delta * right;

    return left;
}

}  // namespace null_engine
