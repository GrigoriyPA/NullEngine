#pragma once

#include <memory>

namespace null_engine::util {

template <typename Derived>
class UniqueConstructable {
public:
    template <typename... Args>
    static std::unique_ptr<Derived> Make(Args&&... args) {
        return std::make_unique<Derived>(std::forward<Args>(args)...);
    }
};

}  // namespace null_engine::util
