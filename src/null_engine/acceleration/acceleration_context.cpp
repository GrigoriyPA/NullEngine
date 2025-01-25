#include "acceleration_context.hpp"

#include <fmt/core.h>

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/context.hpp>

namespace null_engine::multithread {

AccelerationContext AccelerationContext::Create() {
    return AccelerationContext(compute::opengl_create_shared_context());
}

std::string AccelerationContext::GetDeviceDescription() const {
    std::string device_type;
    switch (device_.type()) {
        case CL_DEVICE_TYPE_CPU:
            device_type = "CPU";
            break;

        case CL_DEVICE_TYPE_GPU:
            device_type = "GPU";
            break;

        default:
            device_type = "OTHER";
            break;
    }

    return fmt::format(
        "Device type: {}\nDevice version: {}\nDevice name: {}", device_type, device_.version(), device_.name()
    );
}

compute::context AccelerationContext::GetContext() const {
    return context_;
}

compute::command_queue AccelerationContext::GetQueue() const {
    return queue_;
}

AccelerationContext::AccelerationContext(compute::context context)
    : context_(std::move(context))
    , device_(context_.get_device())
    , queue_(context_, device_) {
}

}  // namespace null_engine::multithread
