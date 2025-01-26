#pragma once

#include "texture.hpp"

namespace null_engine {

struct Material {
    TextureView diffuse_tex;
    TextureView specular_tex;
    TextureView emission_tex;
    FloatType shininess = 1.0;
};

}  // namespace null_engine
