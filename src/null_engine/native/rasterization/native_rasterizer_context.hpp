#pragma once

#include <geometry/constants.hpp>

#include <cstdint>
#include <vector>

namespace null_engine::native {

//
// Rasterization context which contains draw and depth buffers
//
struct RasterizerContext {
public:
    RasterizerContext(int64_t view_width, int64_t view_height);

public:
    // Size in pixels
    int64_t view_width;
    int64_t view_height;

    // Output color buffer with size 4 * view_width * view_height
    // (4 color coordinates - R, G, B, A)
    std::vector<uint8_t> colors_buffer;

    // Depth test buffer with size view_width * view_height
    std::vector<util::FloatType> depth_buffer;
};

}  // namespace null_engine::native
