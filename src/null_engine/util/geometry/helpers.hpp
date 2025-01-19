#pragma once

#include "constants.hpp"

namespace null_engine {

bool Equal(FloatType left, FloatType right);

bool Less(FloatType left, FloatType right);

FloatType Module(FloatType left, FloatType right);

FloatType Clamp(FloatType value, FloatType min_value, FloatType max_value);

}  // namespace null_engine
