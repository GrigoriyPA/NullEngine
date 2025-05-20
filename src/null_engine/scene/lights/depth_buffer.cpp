#include "depth_buffer.hpp"

namespace null_engine::detail {

DepthBuffer::DepthBuffer(const ShadowInfo& shadow_info, const float* depth)
    : Base(shadow_info.shadow_width, shadow_info.shadow_height, depth, 1.0) {
}

}  // namespace null_engine::detail
