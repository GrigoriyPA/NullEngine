#pragma once

#include <boost/compute/command_queue.hpp>
#include <null_engine/util/geometry/vector.hpp>

namespace null_engine::multithread::detail {

namespace compute = boost::compute;

void BuildProgram(compute::program& program);

void RunKernel(
    compute::command_queue& queue, const compute::kernel& kernel, cl_int2 global_work_size, cl_int2 local_work_size
);

cl_float3 Vec3ToCl(Vec3 vactor);

cl_float4 Vec4ToCl(Vec4 vactor);

}  // namespace null_engine::multithread::detail
