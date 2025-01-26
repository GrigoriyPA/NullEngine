#pragma once

#include <boost/compute/command_queue.hpp>

namespace null_engine::multithread::detail {

namespace compute = boost::compute;

void BuildProgram(compute::program& program);

void RunKernel(
    compute::command_queue& queue, const compute::kernel& kernel, cl_int2 global_work_size, cl_int2 local_work_size
);

}  // namespace null_engine::multithread::detail
