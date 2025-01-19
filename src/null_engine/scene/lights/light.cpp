#include "light.hpp"

#include <null_engine/drawable_objects/primitive_objects.hpp>

namespace null_engine {

namespace {

Vec3 GetSpecularColor(
    const Vec3& light_dir, const LightStrength& lignt_settings, const LightingMaterialSettings& material_settings
) {
    const Vec3 halfway_dir = (light_dir + material_settings.view_direction).Normalize();
    const auto normal_diff = std::max(halfway_dir.ScalarProd(material_settings.normal), 0.0);
    const auto spec = std::pow(normal_diff, material_settings.shininess);
    return lignt_settings.specular * spec * material_settings.specular_color;
}

FloatType GetAttenuation(
    const Vec3& light_pos, const AttenuationSettings& lignt_settings, const LightingMaterialSettings& material_settings
) {
    const auto distance = (light_pos - material_settings.frag_pos).Length();
    return 1.0 / (lignt_settings.constant + lignt_settings.linear * distance +
                  lignt_settings.quadratic * distance * distance);
}

}  // anonymous namespace

AmbientLight::AmbientLight(FloatType strength)
    : strength_(strength) {
}

Vec3 AmbientLight::CalculateLighting(const LightingMaterialSettings& settings) const {
    return strength_ * settings.diffuse_color;
}

DirectLight::DirectLight(const Vec3& direction, const LightStrength& strength)
    : inversed_direction_(-Vec3::Normalize(direction))
    , strength_(strength) {
}

Vec3 DirectLight::CalculateLighting(const LightingMaterialSettings& settings) const {
    Vec3 color = strength_.ambient * settings.diffuse_color;

    const auto normal_diff = inversed_direction_.ScalarProd(settings.normal);
    if (normal_diff < 0.0) {
        return color;
    }

    color += strength_.diffuse * normal_diff * settings.diffuse_color;
    color += GetSpecularColor(inversed_direction_, strength_, settings);

    return color;
}

VerticesObject DirectLight::VisualizeLight(Vec3 position, Vec3 color, FloatType scale) const {
    auto result = CreateDirectLightVisualization(color);

    const auto direction = -inversed_direction_;
    const auto horizon = direction.Horizon();
    const auto vertical = horizon.VectorProd(direction);
    result.Transform(Mat4::Translation(position) * Mat4::Basis(horizon, vertical, direction) * Mat4::Scale(scale));

    return result;
}

PointLight::PointLight(const Vec3& position, const LightStrength& strength, const AttenuationSettings& attenuation)
    : position_(position)
    , strength_(strength)
    , attenuation_(attenuation) {
}

Vec3 PointLight::CalculateLighting(const LightingMaterialSettings& settings) const {
    const auto attenuation = GetAttenuation(position_, attenuation_, settings);
    Vec3 color = strength_.ambient * settings.diffuse_color * attenuation;

    Vec3 light_dir = position_ - settings.frag_pos;
    if (light_dir.IsZero()) {
        return color;
    }
    light_dir.Normalize();

    const auto normal_diff = light_dir.ScalarProd(settings.normal);
    if (normal_diff < 0.0) {
        return color;
    }

    color += strength_.diffuse * normal_diff * settings.diffuse_color * attenuation;
    color += GetSpecularColor(light_dir, strength_, settings) * attenuation;

    return color;
}

VerticesObject PointLight::VisualizeLight(Vec3 color, FloatType scale) const {
    auto result = CreatePointLightVisualization(color);

    result.Transform(Mat4::Translation(position_) * Mat4::Scale(scale));

    return result;
}

}  // namespace null_engine
