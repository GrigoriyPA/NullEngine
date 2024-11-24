#include "native_rasterizer_context.hpp"

namespace null_engine::native {

//// RasterizerContext

RasterizerContext::RasterizerContext(int64_t view_width, int64_t view_height)
    : view_width(view_width)
    , view_height(view_height)
    , colors_buffer(4 * view_width * view_height, 0)
    , depth_buffer(view_width * view_height, -1.0) {
}

}  // namespace null_engine::native
