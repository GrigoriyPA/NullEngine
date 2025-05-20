#include "depth_buffer.hpp"

namespace null_engine::detail {

DepthBuffer::DepthBuffer(const ShadowInfo& shadow_info, const float* depth)
    : Base(Width{shadow_info.shadow_width}, Height{shadow_info.shadow_height}, depth, 1.0) {
}

}  // namespace null_engine::detail
