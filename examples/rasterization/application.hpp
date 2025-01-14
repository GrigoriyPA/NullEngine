#pragma once

#include <memory>

namespace null_engine::tests {

class Application {
public:
    Application();

    Application(const Application& other) = delete;
    Application& operator=(const Application& other) = delete;

    ~Application();

    void Run();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace null_engine::tests
