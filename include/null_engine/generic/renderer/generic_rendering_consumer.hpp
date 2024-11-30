#pragma once

#include <memory>

namespace null_engine::generic {

//
// Common interface for all rendering consumers
//
class RenderingConsumer {
public:
    using Ptr = std::shared_ptr<RenderingConsumer>;
};

}  // namespace null_engine::generic