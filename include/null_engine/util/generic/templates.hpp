#pragma once

#include <memory>

namespace null_engine::util {

template <typename Derived>
class SharedConstructable {
public:
    template <typename... Args>
    static std::shared_ptr<Derived> Make(Args&&... args) {
        return std::make_shared<Derived>(std::forward<Args>(args)...);
    }
};

}  // namespace null_engine::util
