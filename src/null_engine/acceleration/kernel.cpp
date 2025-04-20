#include "kernel.hpp"

namespace null_engine::multithread::detail {

Kernel::Args::Args(size_t offset, compute::kernel& kernel)
    : offset_(offset)
    , kernel_(&kernel) {
}

Kernel::Args& Kernel::Args::SetData(size_t index, size_t size, const void* data) {
    kernel_->set_arg(offset_ + index, size, data);
    return *this;
}

Kernel::Kernel(const std::string& kernel_name, Program&& kernel_program, AccelerationContext context)
    : kernel_program_(std::move(kernel_program))
    , kernel_(kernel_program_.BuildKernel(kernel_name, context))
    , context_(context) {
}

Kernel::Args Kernel::MutableArgs(size_t offset) {
    return Args(offset, kernel_);
}

}  // namespace null_engine::multithread::detail
