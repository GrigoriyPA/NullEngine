#include "native_rasterizer_context.hpp"

namespace null_engine::native {

RasterizerContext::RasterizerContext(uint64_t view_width, uint64_t view_height)
    : view_width(view_width)
    , view_height(view_height) {
    Rewind();
}

void RasterizerContext::Rewind() {
    colors_buffer.assign(4 * view_width * view_height, 0);
    depth_buffer.assign(view_width * view_height, 1.0);
}

}  // namespace null_engine::native
