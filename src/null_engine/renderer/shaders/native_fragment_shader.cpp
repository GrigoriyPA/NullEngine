#include "native_fragment_shader.hpp"

#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <optional>

namespace null_engine::native::detail {

Vec4 NoopFragmentShader::GetPointColor(const InterpolationParams& params, bool& discard) const {
    return params.color;
}

Vec3 MainFragmentShader::GetViewPos() const {
    return view_pos_;
}

Vec4 MainFragmentShader::GetPointColor(const InterpolationParams& params, bool& discard) const {
    Vec4 diffuse_color = params.color;
    if (material_.diffuse_tex) {
        diffuse_color = material_.diffuse_tex->GetColor(params.tex_coords);
    }

    const auto alpha = diffuse_color.w();
    if (Equal(alpha, 0.0)) {
        discard = true;
        return diffuse_color;
    }

    Vec3 result_color(0.0, 0.0, 0.0);
    if (material_.emission_tex) {
        result_color = Vec4ToVec3(material_.emission_tex->GetColor(params.tex_coords));
    }

    if (!number_lights_ || params.normal.isZero()) {
        return Vec3ToVec4(Vec4ToVec3(diffuse_color) + result_color, alpha);
    }

    LightingMaterialSettings light_settings{
        .frag_pos = params.frag_pos,
        .view_direction = (view_pos_ - params.frag_pos).normalized(),
        .normal = params.normal.normalized(),
        .diffuse_color = Vec4ToVec3(diffuse_color),
        .shadow = material_.shadow
    };

    if (material_.specular_tex) {
        light_settings.specular_color = Vec4ToVec3(material_.specular_tex->GetColor(params.tex_coords));
        light_settings.shininess = material_.shininess;
    }

    for (uint32_t i = 0; i < kMaxNumberLights; ++i) {
        if (i == number_lights_) {
            break;
        }

        const auto& light_info = lights_[i];
        result_color += light_info.light.CalculateLighting(light_settings, light_info.depth);
    }
    return Vec3ToVec4(result_color, alpha);
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
