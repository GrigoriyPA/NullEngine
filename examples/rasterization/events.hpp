#pragma once

#include <SFML/OpenGL.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <variant>
#include <vector>

namespace null_engine::tests {

using TextureData = std::vector<uint8_t>;

struct DrawViewEvent {
    FloatType delta_time;
    std::variant<TextureData, GLuint> render_texture;
};

}  // namespace null_engine::tests
