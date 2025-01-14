#include <null_engine/util/generic/validation.hpp>

#include "quick_start_application.hpp"

int main() {
    try {
        null_engine::example::NativeApplication application;
        application.Run();
    } catch (...) {
        null_engine::HandleException();
    }
    return 0;
}
