#include "helpers.hpp"

#include <cstdlib>

namespace null_engine::util {

bool Equal(FloatType left, FloatType right) {
    return std::abs(right - left) <= kEps;
}

}  // namespace null_engine::util
