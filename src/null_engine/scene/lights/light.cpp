#include "light.hpp"

#include <CL/cl_platform.h>

#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/drawable_objects/primitive_objects.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <numbers>
#include <sstream>

namespace null_engine {

using namespace multithread::detail;

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

namespace multithread::detail {

namespace {

cl_float3 GetClStrength(const LightStrength& strength) {
    return {
        .x = strength.ambient,
        .y = strength.diffuse,
        .z = strength.specular,
    };
}

cl_float3 GetClAttenuation(const AttenuationSettings& attenuation) {
    return {
        .x = attenuation.constant,
        .y = attenuation.linear,
        .z = attenuation.quadratic,
    };
}

}  // anonymous namespace

std::string GetLightsSource() {
    std::stringstream lights_source;
    lights_source << BOOST_COMPUTE_STRINGIZE_SOURCE(
        enum LightType{
            LT_NONE,
            LT_AMBIENT,
            LT_DIRECT,
            LT_POINT,
            LT_SPOT,
        };

        typedef struct {
            int light_type;
            float3 strength;
            float3 attenuation;
            float3 position;
            float3 direction;
            float2 angle;
        } LightDescription;

        typedef struct {
            float3 frag_pos;
            float3 view_direction;
            float3 normal;
            float3 diffuse_color;
            float3 specular_color;
            float shininess;
        } LightingMaterialSettings;
    );

    lights_source << AmbientLight::GetKernelSource();

    lights_source << BOOST_COMPUTE_STRINGIZE_SOURCE(
        float3 CalculateLighting(LightDescription * light_desc, LightingMaterialSettings * material_desc) {
            switch (light_desc->light_type) {
                case LT_AMBIENT:
                    return CalculateAmbientLight(light_desc->strength.x, material_desc);
                default:
                    return (float3)(0.0f, 0.0f, 0.0f);
            }
        }
    );

    return lights_source.str();
}

}  // namespace multithread::detail

AmbientLight::AmbientLight(FloatType strength)
    : strength_(strength) {
}

Vec3 AmbientLight::CalculateLighting(const LightingMaterialSettings& material) const {
    return strength_ * material.diffuse_color;
}

std::string AmbientLight::GetKernelSource() {
    return BOOST_COMPUTE_STRINGIZE_SOURCE(

        float3 CalculateAmbientLight(float strength, LightingMaterialSettings* material) {
            return strength * material->diffuse_color;
        }
    );
}

AmbientLight::LightDescription AmbientLight::GetDescription() const {
    return {
        .light_type = LightDescription::LT_AMBIENT,
        .strength = {.x = strength_},
    };
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

std::string DirectLight::GetKernelSource() {
    return "";
}

DirectLight::LightDescription DirectLight::GetDescription() const {
    return {
        .light_type = LightDescription::LT_DIRECT,
        .strength = GetClStrength(strength_),
        .direction = Vec3ToCl(inversed_direction_)
    };
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

std::string PointLight::GetKernelSource() {
    return "";
}

PointLight::LightDescription PointLight::GetDescription() const {
    return {
        .light_type = LightDescription::LT_POINT,
        .strength = GetClStrength(strength_),
        .attenuation = GetClAttenuation(attenuation_),
        .position = Vec3ToCl(position_)
    };
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

std::string SpotLight::GetKernelSource() {
    return "";
}

SpotLight::LightDescription SpotLight::GetDescription() const {
    return {
        .light_type = LightDescription::LT_SPOT,
        .strength = GetClStrength(strength_),
        .attenuation = GetClAttenuation(attenuation_),
        .position = Vec3ToCl(position_),
        .direction = Vec3ToCl(inversed_direction_),
        .angle = {.x = cut_in_, .y = cut_out_}
    };
}

VerticesObject SpotLight::VisualizeLight(Vec3 color, FloatType scale) const {
    return VisualizeDirectedLight(position_, -inversed_direction_, color, scale);
}

void SpotLight::ApplyTransform(const Transform& transform) {
    inversed_direction_ = (transform.linear() * inversed_direction_).normalized();
    position_ = transform * position_;
}

}  // namespace null_engine
