#pragma once

#include <boost/compute/command_queue.hpp>

namespace null_engine::multithread {

namespace compute = boost::compute;

class AccelerationContext {
public:
    static AccelerationContext Create();

    std::string GetDeviceDescription() const;

    compute::context GetContext() const;

    compute::command_queue GetQueue() const;

private:
    explicit AccelerationContext(compute::context context);

    compute::context context_;
    compute::device device_;
    compute::command_queue queue_;
};

}  // namespace null_engine::multithread
