#include "native_fragment_shader.hpp"

namespace null_engine::native::detail {

Vec3 NoopFragmentShader::GetPointColor(const InterpolationParams& params) const {
    return params.color;
}

Vec3 MainFragmentShader::GetViewPos() const {
    return view_pos_;
}

Vec3 MainFragmentShader::GetPointColor(const InterpolationParams& params) const {
    Vec3 diffuse_color = params.color;
    if (material_.diffuse_tex) {
        diffuse_color = material_.diffuse_tex->GetColor(params.tex_coords);
    }

    Vec3 result_color(0.0, 0.0, 0.0);
    if (material_.emission_tex) {
        result_color = material_.emission_tex->GetColor(params.tex_coords);
    }

    if (!number_lights_ || params.normal.isZero()) {
        return diffuse_color + result_color;
    }

    LightingMaterialSettings light_settings{
        .frag_pos = params.frag_pos,
        .view_direction = (view_pos_ - params.frag_pos).normalized(),
        .normal = params.normal.normalized(),
        .diffuse_color = diffuse_color,
        .shadow = material_.shadow
    };

    if (material_.specular_tex) {
        light_settings.specular_color = material_.specular_tex->GetColor(params.tex_coords);
        light_settings.shininess = material_.shininess;
    }

    for (uint32_t i = 0; i < kMaxNumberLights; ++i) {
        if (i == number_lights_) {
            break;
        }

        const auto& light_info = lights_[i];
        result_color += light_info.light.CalculateLighting(light_settings, light_info.depth);
    }
    return result_color;
}

void MainFragmentShader::SetViewPos(Vec3 view_pos) {
    view_pos_ = view_pos;
}

void MainFragmentShader::SetMaterial(const Material& material) {
    material_ = material;
}

void MainFragmentShader::SetLights(const std::vector<LightSettings>& lights) {
    assert(lights.size() <= kMaxNumberLights && "Too many lights provided");

    number_lights_ = lights.size();
    for (uint32_t i = 0; const auto& light : lights) {
        lights_[i++] = light;
    }
}

}  // namespace null_engine::native::detail
