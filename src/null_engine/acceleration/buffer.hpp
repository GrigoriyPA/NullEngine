#pragma once

#include <boost/compute/buffer.hpp>
#include <functional>

#include "acceleration_context.hpp"

namespace null_engine::multithread::detail {

template <typename DeviceValue>
class DynamicBuffer {
public:
    template <typename HostValue>
    using MapFunc = std::function<DeviceValue(const HostValue& value)>;

    explicit DynamicBuffer(AccelerationContext context)
        : context_(context)
        , buffer_(context.GetContext(), 0) {
    }

    const compute::buffer& GetBuffer() const {
        return buffer_;
    }

    template <typename HostValue>
    void Assign(const std::vector<HostValue>& host_values, MapFunc<HostValue> map_func) {
        values_.clear();
        values_.reserve(host_values.size());
        for (const auto& value : host_values) {
            values_.emplace_back(map_func(value));
        }

        const auto size = values_.size() * sizeof(DeviceValue);
        if (buffer_.size() < size) {
            buffer_ = compute::buffer(context_.GetContext(), size);
        }
        context_.GetQueue().enqueue_write_buffer(buffer_, 0, size, values_.data());
    }

private:
    AccelerationContext context_;
    compute::buffer buffer_;
    std::vector<DeviceValue> values_;
};

}  // namespace null_engine::multithread::detail
