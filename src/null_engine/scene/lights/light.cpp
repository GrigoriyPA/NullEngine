#include "light.hpp"

#include <null_engine/drawable_objects/primitive_objects.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <numbers>

namespace null_engine {

namespace {

Vec3 GetSpecularColor(Vec3 light_dir, const LightStrength& settings, const LightingMaterialSettings& material) {
    if (Equal(material.shininess, 0.0)) {
        return Vec3(0.0, 0.0, 0.0);
    }

    const auto normal_diff =
        std::max<FloatType>((light_dir + material.view_direction).normalized().dot(material.normal), 0.0);
    const auto spec = std::pow(normal_diff, material.shininess);
    return settings.specular * spec * material.specular_color;
}

FloatType GetAttenuation(
    const Vec3& light_pos, const AttenuationSettings& settings, const LightingMaterialSettings& material
) {
    const auto distance = (light_pos - material.frag_pos).norm();
    return 1.0 / (settings.constant + settings.linear * distance + settings.quadratic * distance * distance);
}

VerticesObject VisualizeDirectedLight(Vec3 position, Vec3 direction, Vec3 color, FloatType scale) {
    auto result = CreateDirectLightVisualization(color);

    const auto horizon = Horizon(direction);
    const auto vertical = VectorProd(horizon, direction);
    result.ApplyTransform(Translation(position) * Basis(horizon, vertical, direction) * Scale(scale));

    return result;
}

}  // anonymous namespace

AmbientLight::AmbientLight(FloatType strength)
    : strength_(strength) {
}

Vec3 AmbientLight::CalculateLighting(const LightingMaterialSettings& material) const {
    return strength_ * material.diffuse_color;
}

void AmbientLight::ApplyTransform(const Transform& transform) {
}

DirectLight::DirectLight(Vec3 direction, const LightStrength& strength)
    : inversed_direction_(-direction.normalized())
    , strength_(strength) {
}

Vec3 DirectLight::CalculateLighting(const LightingMaterialSettings& material) const {
    Vec3 color = strength_.ambient * material.diffuse_color;

    const auto normal_diff = inversed_direction_.dot(material.normal);
    if (normal_diff < 0.0) {
        return color;
    }

    color += strength_.diffuse * normal_diff * material.diffuse_color;
    color += GetSpecularColor(inversed_direction_, strength_, material);

    return color;
}

VerticesObject DirectLight::VisualizeLight(Vec3 position, Vec3 color, FloatType scale) const {
    return VisualizeDirectedLight(position, -inversed_direction_, color, scale);
}

void DirectLight::ApplyTransform(const Transform& transform) {
    inversed_direction_ = (transform.linear() * inversed_direction_).normalized();
}

PointLight::PointLight(Vec3 position, const LightStrength& strength, const AttenuationSettings& attenuation)
    : position_(position)
    , strength_(strength)
    , attenuation_(attenuation) {
}

Vec3 PointLight::CalculateLighting(const LightingMaterialSettings& material) const {
    const auto attenuation = GetAttenuation(position_, attenuation_, material);
    Vec3 color = strength_.ambient * material.diffuse_color * attenuation;

    Vec3 light_dir = position_ - material.frag_pos;
    if (light_dir.isZero()) {
        return color;
    }
    light_dir.normalize();

    const auto normal_diff = light_dir.dot(material.normal);
    if (normal_diff < 0.0) {
        return color;
    }

    color += strength_.diffuse * normal_diff * material.diffuse_color * attenuation;
    color += GetSpecularColor(light_dir, strength_, material) * attenuation;

    return color;
}

VerticesObject PointLight::VisualizeLight(Vec3 color, FloatType scale) const {
    auto result = CreatePointLightVisualization(color);

    result.ApplyTransform(Translation(position_) * Scale(scale));

    return result;
}

void PointLight::ApplyTransform(const Transform& transform) {
    position_ = transform * position_;
}

SpotLight::SpotLight(const Settings& settings, const LightStrength& strength, const AttenuationSettings& attenuation)
    : position_(settings.position)
    , inversed_direction_(-settings.direction.normalized())
    , strength_(strength)
    , attenuation_(attenuation)
    , cut_in_(std::cos(settings.light_angle / 2.0))
    , cut_out_(std::cos(settings.light_angle * settings.light_angle_ratio / 2.0)) {
    assert(Less(0.0, settings.light_angle) && "Spot light angle shuld be at least zero");
    assert(Less(1.0, settings.light_angle_ratio) && "Spot light angle ratio should be at least one");

    const auto max_angle = std::numbers::pi;
    assert(Less(settings.light_angle, max_angle) && "Spot light angle shuld be at most pi");
    assert(Less(settings.light_angle * settings.light_angle_ratio, max_angle) && "Spot light angle ratio too large");
}

Vec3 SpotLight::CalculateLighting(const LightingMaterialSettings& material) const {
    auto attenuation = GetAttenuation(position_, attenuation_, material);
    Vec3 color = strength_.ambient * material.diffuse_color * attenuation;

    Vec3 light_dir = position_ - material.frag_pos;
    if (light_dir.isZero()) {
        return color;
    }
    light_dir.normalize();

    const auto normal_diff = light_dir.dot(material.normal);
    if (normal_diff < 0.0) {
        return color;
    }

    const auto theta = light_dir.dot(inversed_direction_);
    attenuation *= Clamp((theta - cut_out_) / (cut_in_ - cut_out_), 0.0, 1.0);

    color += strength_.diffuse * normal_diff * material.diffuse_color * attenuation;
    color += GetSpecularColor(light_dir, strength_, material) * attenuation;

    return color;
}

VerticesObject SpotLight::VisualizeLight(Vec3 color, FloatType scale) const {
    return VisualizeDirectedLight(position_, -inversed_direction_, color, scale);
}

void SpotLight::ApplyTransform(const Transform& transform) {
    inversed_direction_ = (transform.linear() * inversed_direction_).normalized();
    position_ = transform * position_;
}

}  // namespace null_engine
