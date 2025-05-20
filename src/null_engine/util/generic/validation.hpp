#pragma once

#include <string>

namespace null_engine {

void Ensure(bool condition, const std::string& message);

void HandleException();

}  // namespace null_engine
