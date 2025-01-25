#include "helpers.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace null_engine {

FloatType Module(FloatType left, FloatType right) {
    assert(right > kEps && "Division by zero");

    const int64_t delta = std::floor(left / right);
    left -= delta * right;

    return left;
}

FloatType Clamp(FloatType value, FloatType min_value, FloatType max_value) {
    assert(min_value <= max_value && "Invalid clamp parameters");
    return std::min(max_value, std::max(min_value, value));
}

}  // namespace null_engine
