#pragma once

#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>

namespace null_engine::util {

//
// Common error for all null-engine funstions
//
class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& message, std::source_location location);

    std::string GetMessage() const;
    std::source_location GetLocation() const;

public:
    const char* what() const override;

private:
    std::source_location location_;
    std::string error_;
};

// Throws RuntimeError if condition is not true
void Ensure(bool condition, const std::string& message,
            std::source_location location = std::source_location::current());

void Ensure(bool condition, const std::stringstream& message,
            std::source_location location = std::source_location::current());

}  // namespace null_engine::util
