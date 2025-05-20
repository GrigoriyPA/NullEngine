#include "helpers.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace null_engine {

float Module(float value, float modulus) {
    assert(modulus > kEps && "Division by zero");

    const int64_t delta = std::floor(value / modulus);
    value -= delta * modulus;

    return value;
}

float Clamp(float value, float min_value, float max_value) {
    assert(min_value <= max_value && "Invalid clamp parameters");
    return std::min(max_value, std::max(min_value, value));
}

}  // namespace null_engine
