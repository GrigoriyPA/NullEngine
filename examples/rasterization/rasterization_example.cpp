#include <null_engine/util/generic/validation.hpp>

#include "application.hpp"

int main() {
    try {
        null_engine::tests::Application application;
        application.Run();
    } catch (...) {
        null_engine::HandleException();
    }
    return 0;
}
