#pragma once

#include "texture.hpp"

namespace null_engine {

struct Material {
    std::optional<TextureView> diffuse_tex;
    std::optional<TextureView> specular_tex;
    std::optional<TextureView> emission_tex;
    FloatType shininess = 1.0;
};

}  // namespace null_engine
