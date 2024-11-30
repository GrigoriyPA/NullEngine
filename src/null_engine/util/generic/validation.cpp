#include "validation.hpp"

#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>

namespace null_engine::util {

//// NERuntimeError

RuntimeError::RuntimeError(const std::string& message, std::source_location location)
    : runtime_error(message)
    , location_(location)
    , error_((std::stringstream() << "NullEngine runtime error: " << message << "\nOccurred in file "
                                  << location_.file_name() << ", in function" << location_.file_name() << " on line "
                                  << location_.line())
                 .str()) {
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

/// Validation functions

void Ensure(bool condition, const std::string& message, std::source_location location) {
    if (!condition) {
        throw RuntimeError(message, location);
    }
}

void Ensure(bool condition, const std::stringstream& message, std::source_location location) {
    Ensure(condition, message.str(), location);
}

}  // namespace null_engine::util
