#pragma once

#include <SFML/OpenGL.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <variant>
#include <vector>

namespace null_engine::example {

using TextureData = std::vector<uint8_t>;

struct DrawViewEvent {
    float delta_time;
    std::variant<TextureData, GLuint> render_texture;
};

enum class MultithreadingMode { Enabled, Disabled };
enum class MouseControlMode { Enabled, Disabled };

}  // namespace null_engine::example
