#include "fragment_shader.hpp"

namespace null_engine::detail {

Vec3 FragmentShader::GetViewPos() const {
    return view_pos_;
}

Vec3 FragmentShader::GetPointColor(const InterpolationParams& params) const {
    Vec3 diffuse_color = params.color;
    if (material_.diffuse_tex.HasTexture()) {
        diffuse_color = material_.diffuse_tex.GetColor(params.tex_coords);
    }

    Vec3 result_color(0.0, 0.0, 0.0);
    if (material_.emission_tex.HasTexture()) {
        result_color = material_.emission_tex.GetColor(params.tex_coords);
    }

    if (!number_lights_ || params.normal.isZero()) {
        return diffuse_color + result_color;
    }

    LightingMaterialSettings light_settings{
        .frag_pos = params.frag_pos,
        .view_direction = (view_pos_ - params.frag_pos).normalized(),
        .normal = params.normal.normalized(),
        .diffuse_color = diffuse_color
    };

    if (material_.specular_tex.HasTexture()) {
        light_settings.specular_color = material_.specular_tex.GetColor(params.tex_coords);
        light_settings.shininess = material_.shininess;
    }

    for (uint32_t i = 0; i < kMaxNumberLights; ++i) {
        if (i == number_lights_) {
            break;
        }
        result_color += lights_[0].CalculateLighting(light_settings);
    }
    return result_color;
}

void FragmentShader::SetViewPos(Vec3 view_pos) {
    view_pos_ = view_pos;
}

void FragmentShader::SetMaterial(const Material& material) {
    material_ = material;
}

void FragmentShader::SetLights(const std::vector<AnyLight>& lights) {
    assert(lights.size() <= kMaxNumberLights && "Too many lights provided");

    number_lights_ = lights.size();
    for (uint32_t i = 0; const auto& light : lights) {
        lights_[i++] = light;
    }
}

}  // namespace null_engine::detail
