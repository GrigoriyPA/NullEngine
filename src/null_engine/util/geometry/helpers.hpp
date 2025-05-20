#pragma once

#include <cstdlib>

#include "constants.hpp"

namespace null_engine {

inline bool Equal(float left, float right) {
    return std::abs(right - left) <= kEps;
}

inline bool Less(float left, float right) {
    return left <= right && !Equal(left, right);
}

float Module(float value, float modulus);

float Clamp(float value, float min_value, float max_value);

}  // namespace null_engine
