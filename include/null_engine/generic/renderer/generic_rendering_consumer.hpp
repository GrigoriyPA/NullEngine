#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace null_engine::generic {

//
// Common interface for all rendering consumers
//
class RenderingConsumer {
public:
    using Ptr = std::shared_ptr<RenderingConsumer>;

public:
    // texture -- array of pixel colors with size 4 * view_width * view_height
    // (4 color coordinates - R, G, B, A)
    virtual void OnRenderedTexture(const std::vector<uint8_t>& texture) = 0;
};

}  // namespace null_engine::generic
