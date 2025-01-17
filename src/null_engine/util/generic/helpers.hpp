#pragma once

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

}  // namespace null_engine
