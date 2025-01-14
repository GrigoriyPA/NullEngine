#pragma once

#include <source_location>
#include <stdexcept>
#include <string>

namespace null_engine {

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

void Ensure(
    bool condition, const std::string& message, std::source_location location = std::source_location::current()
);

void HandleException();

}  // namespace null_engine
