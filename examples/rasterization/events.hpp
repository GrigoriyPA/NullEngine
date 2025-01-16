#pragma once

#include <null_engine/util/geometry/constants.hpp>
#include <vector>

namespace null_engine::tests {

using TextureData = std::vector<uint8_t>;

struct DrawViewEvent {
    FloatType delta_time;
    const TextureData& render_texture;
};

}  // namespace null_engine::tests
