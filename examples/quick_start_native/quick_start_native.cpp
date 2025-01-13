#include <iostream>

#include "quick_start_application.hpp"

int main() {
    try {
        null_engine::example::NativeApplication application;
        application.Run();
    } catch (const std::exception& error) {
        std::cerr << "Got unexpected exception:\n" << error.what();
    } catch (...) {
        std::cerr << "Got unexpected exception\n";
    }

    return 0;
}
