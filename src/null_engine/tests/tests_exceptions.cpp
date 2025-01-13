#include "tests_exceptions.hpp"

#include <iostream>
#include <null_engine/util/generic/validation.hpp>

namespace null_engine::tests {

void HandleException() {
    try {
        throw;
    } catch (const std::exception& exception) {
        std::cerr << "Got exception:\n" << exception.what();
    } catch (...) {
        std::cerr << "Got unknown exception\n";
    }
}

}  // namespace null_engine::tests
