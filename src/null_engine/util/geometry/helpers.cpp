#include <null_engine/util/geometry/helpers.hpp>

#include <cstdlib>

namespace null_engine::util {

//// Geometry functions

bool Equal(FloatType left, FloatType right) {
    return std::abs(right - left) <= kEps;
}

}  // namespace null_engine::util
