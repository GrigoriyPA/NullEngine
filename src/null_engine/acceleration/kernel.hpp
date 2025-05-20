#pragma once

#include "helpers.hpp"
#include "program.hpp"

namespace null_engine::multithread::detail {

template <typename T>
concept KernelSizeType = std::is_same_v<T, cl_int> || std::is_same_v<T, cl_int2>;

class Kernel {
public:
    class Args {
        friend class Kernel;

    public:
        template <typename T>
        Args& SetVal(size_t index, const T& arg) {
            kernel_->set_arg(offset_ + index, arg);
            return *this;
        }

        template <typename T>
        Args& SetData(size_t index, const T& arg) {
            SetData(index, sizeof(T), &arg);
            return *this;
        }

        Args& SetData(size_t index, size_t size, const void* data);

    private:
        Args(size_t offset, compute::kernel& kernel);

        size_t offset_ = 0;
        compute::kernel* kernel_;
    };

    Kernel(const std::string& kernel_name, Program&& kernel_program, AccelerationContext context);

    Args MutableArgs(size_t offset = 0);

    template <KernelSizeType T>
    void Run(T global_size, T loacl_size) {
        RunKernel(context_.GetQueue(), kernel_, global_size, loacl_size);
    }

private:
    Program kernel_program_;
    compute::kernel kernel_;
    AccelerationContext context_;
};

}  // namespace null_engine::multithread::detail
