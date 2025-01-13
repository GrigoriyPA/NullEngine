#include <null_engine/tests/tests_exceptions.hpp>

#include "quick_start_application.hpp"

int main() {
    try {
        null_engine::example::NativeApplication application;
        application.Run();
    } catch (...) {
        null_engine::tests::HandleException();
    }
    return 0;
}
