#pragma once

namespace null_engine::util {

using FloatType = double;

constexpr FloatType kPi = 3.141592653589793;
constexpr FloatType kEps = 1e-6;

// Equality comprassion with accurate to kEps
bool Equal(FloatType left, FloatType right);

}  // namespace null_engine::util
