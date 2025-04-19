#include "validation.hpp"

#include <fmt/core.h>

#include <iostream>
#include <stdexcept>

namespace null_engine {

void Ensure(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(fmt::format("NullEngine runtime error: {}", message));
    }
}

void HandleException() {
    try {
        throw;
    } catch (const std::exception& exception) {
        std::cerr << "Got exception:\n" << exception.what();
    } catch (...) {
        std::cerr << "Got unknown exception\n";
    }
}

}  // namespace null_engine
