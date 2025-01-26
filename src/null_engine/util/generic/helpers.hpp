#pragma once

#include <functional>
#include <vector>

namespace null_engine {

template <typename Value>
void SwapRemove(std::vector<Value>& velues, int64_t index) {
    assert(index < velues.size() && "Invalid index for remove");

    if (index + 1 < velues.size()) {
        std::swap(velues[index], velues.back());
    }
    velues.pop_back();
}

template <typename Value>
void SortValues(Value& a, Value& b, Value& c, std::function<bool(const Value&, const Value&)> less) {
    if (less(a, b)) {
        std::swap(a, b);
    }
    if (less(b, c)) {
        std::swap(b, c);
        if (less(a, b)) {
            std::swap(a, b);
        }
    }
}

}  // namespace null_engine
