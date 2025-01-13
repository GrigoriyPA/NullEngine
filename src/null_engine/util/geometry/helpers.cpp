#include "helpers.hpp"

#include <cstdlib>

namespace null_engine::util {

bool Equal(FloatType left, FloatType right) {
    return std::abs(right - left) <= kEps;
}

bool Less(FloatType left, FloatType right) {
    return left <= right && !Equal(left, right);
}

}  // namespace null_engine::util
