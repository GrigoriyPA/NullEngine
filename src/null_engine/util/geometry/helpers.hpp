#pragma once

#include <cstdlib>

#include "constants.hpp"

namespace null_engine {

inline bool Equal(FloatType left, FloatType right) {
    return std::abs(right - left) <= kEps;
}

inline bool Less(FloatType left, FloatType right) {
    return left <= right && !Equal(left, right);
}

FloatType Module(FloatType left, FloatType right);

FloatType Clamp(FloatType value, FloatType min_value, FloatType max_value);

}  // namespace null_engine
