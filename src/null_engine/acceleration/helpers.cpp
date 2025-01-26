#include "helpers.hpp"

#include <fmt/core.h>

#include <boost/compute/utility/dim.hpp>
#include <null_engine/util/generic/validation.hpp>

namespace null_engine::multithread::detail {

namespace {

constexpr cl_int kMaxLocalWorkSize = 256;

}  // anonymous namespace

void BuildProgram(compute::program& program) {
    try {
        program.build();
    } catch (const std::exception& error) {
        Ensure(false, fmt::format("Failed to build program, {}, build log:\n{}", error.what(), program.build_log()));
    }
}

void RunKernel(
    compute::command_queue& queue, const compute::kernel& kernel, cl_int2 global_work_size, cl_int2 local_work_size
) {
    assert(local_work_size.x * local_work_size.y <= kMaxLocalWorkSize && "Loacal work size too large");

    if (const cl_int remainder = global_work_size.x % local_work_size.x) {
        global_work_size.x += local_work_size.x - remainder;
    }
    if (const cl_int remainder = global_work_size.y % local_work_size.y) {
        global_work_size.y += local_work_size.y - remainder;
    }

    queue.enqueue_nd_range_kernel(
        kernel, compute::dim(0, 0), compute::dim(global_work_size.x, global_work_size.y),
        compute::dim(local_work_size.x, local_work_size.y)
    );
}

cl_float3 Vec3ToCl(Vec3 vactor) {
    return {
        .x = static_cast<cl_float>(vactor.x()),
        .y = static_cast<cl_float>(vactor.y()),
        .z = static_cast<cl_float>(vactor.z()),
    };
}

cl_float4 Vec4ToCl(Vec4 vactor) {
    return {
        .x = static_cast<cl_float>(vactor.x()),
        .y = static_cast<cl_float>(vactor.y()),
        .z = static_cast<cl_float>(vactor.z()),
        .w = static_cast<cl_float>(vactor.w()),
    };
}

}  // namespace null_engine::multithread::detail
