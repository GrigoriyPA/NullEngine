#include "validation.hpp"

#include <fmt/core.h>

#include <iostream>

namespace null_engine {

RuntimeError::RuntimeError(const std::string& message, std::source_location location)
    : runtime_error(message)
    , location_(location)
    , error_(fmt::format(
          "NullEngine runtime error: {}\nOccurred in file {}:{}", message, location_.file_name(), location_.line()
      )) {
}

std::string RuntimeError::GetMessage() const {
    return runtime_error::what();
}

std::source_location RuntimeError::GetLocation() const {
    return location_;
}

char const* RuntimeError::what() const {
    return error_.c_str();
}

void Ensure(bool condition, const std::string& message, std::source_location location) {
    if (!condition) {
        throw RuntimeError(message, location);
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
