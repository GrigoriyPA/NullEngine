#pragma once

#include <memory>

namespace null_engine::generic {

//
// Common interface for all rendering backeds
//
class Renderer {
public:
    using Ptr = std::shared_ptr<Renderer>;
};

}  // namespace null_engine::generic
