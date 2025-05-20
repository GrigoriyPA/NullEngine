#pragma once

#include <vector>

namespace null_engine {

template <typename Value>
void SwapRemove(std::vector<Value>& values, int64_t index) {
    assert(index < values.size() && "Invalid index for remove");

    if (index + 1 < values.size()) {
        std::swap(values[index], values.back());
    }
    values.pop_back();
}

template <typename Value, typename Comparator>
void SortValues(Value& a, Value& b, Value& c, Comparator less) {
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
