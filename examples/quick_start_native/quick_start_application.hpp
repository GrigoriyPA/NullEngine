#pragma once

#include <memory>

namespace null_engine::example {

class NativeApplication {
public:
    NativeApplication();

    virtual ~NativeApplication();

    void Run();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace null_engine::example
