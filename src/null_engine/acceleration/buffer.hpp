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

    explicit DynamicBuffer(AccelerationContext context, uint64_t size = 0)
        : context_(context)
        , buffer_(context.GetContext(), size * sizeof(DeviceValue))
        , size_(size) {
    }

    uint64_t GetSize() const {
        return size_;
    }

    const compute::buffer& GetBuffer() const {
        return buffer_;
    }

    void Reserve(uint64_t size) {
        if (size > size_) {
            buffer_ = compute::buffer(context_.GetContext(), size * sizeof(DeviceValue));
            size_ = size;
        }
    }

    template <typename HostValue>
    void Assign(const std::vector<HostValue>& host_values, MapFunc<HostValue> map_func) {
        values_.clear();
        values_.reserve(host_values.size());
        for (const auto& value : host_values) {
            values_.emplace_back(map_func(value));
        }

        Reserve(values_.size());
        context_.GetQueue().enqueue_write_buffer(buffer_, 0, values_.size() * sizeof(DeviceValue), values_.data());
    }

private:
    AccelerationContext context_;
    compute::buffer buffer_;
    std::vector<DeviceValue> values_;
    uint64_t size_;
};

}  // namespace null_engine::multithread::detail
