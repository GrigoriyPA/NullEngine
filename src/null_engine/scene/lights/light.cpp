#include "light.hpp"

#include <CL/cl_platform.h>

#include <boost/compute/utility/source.hpp>
#include <cmath>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/acceleration/program.hpp>
#include <null_engine/drawable_objects/primitive_objects.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/helpers.hpp>
#include <null_engine/util/geometry/matrix.hpp>
#include <null_engine/util/geometry/vector.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>
#include <numbers>
#include <optional>

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

FloatType GetShadow(
    const ProjectiveTransform& shadow_space, const ILight::DepthBuffer& depth, const LightingMaterialSettings& material
) {
    static constexpr FloatType kBias = 0.01;

    if (!material.shadow) {
        return 0.0;
    }

    Vec4 frag_pos = shadow_space * Vec4(material.frag_pos.x(), material.frag_pos.y(), material.frag_pos.z(), 1.0);
    frag_pos /= frag_pos.w();
    if (frag_pos.z() > 1.0) {
        return 0.0;
    }

    FloatType shadow = 0.0;
    const Vec2 tex_coords((frag_pos.x() + 1.0) / 2.0, (1.0 - frag_pos.y()) / 2.0);
    const Vec2 texel_size = depth.GetTexelSize();
    for (int32_t x = -1; x <= 1; ++x) {
        for (int32_t y = -1; y <= 1; ++y) {
            const FloatType tex_depth = depth.GetData(tex_coords + Vec2(x * texel_size.x(), y * texel_size.y()));
            if (frag_pos.z() - kBias > tex_depth) {
                shadow += 1.0;
            }
        }
    }

    return shadow / 9.0;
}

Transform GetOrientationTransform(Vec3 direction) {
    const auto horizon = Horizon(direction).normalized();
    return Basis(horizon, VectorProd(horizon, direction).normalized(), direction.normalized());
}

VerticesObject VisualizeDirectedLight(Vec3 position, Vec3 direction, Vec4 color, FloatType scale) {
    auto result = CreateDirectLightVisualization(color);

    const auto horizon = Horizon(direction);
    const auto vertical = VectorProd(horizon, direction);
    result.ApplyTransform(Translation(position) * Basis(horizon, vertical, direction) * Scale(scale));
    result.SetMaterial({.shadow = false});

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

Program GetLightUtilsProgram() {
    static constexpr std::string_view kLightUtilsSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
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
            float4 shadow_space[4];
            int2 shadow_size;
        } LightDescription;

        typedef struct {
            float3 frag_pos;
            float3 view_direction;
            float3 normal;
            float3 diffuse_color;
            float3 specular_color;
            float shininess;
            bool shadow;
        } LightingMaterialSettings;

        typedef struct {
            const float4* shadow_space;
            int2 shadow_size;
            const float* shadow_map;
        } ShadowSettings;

        float3 GetSpecularColor(float3 light_dir, float strength, const LightingMaterialSettings* material) {
            if (fabs(material->shininess) < kEps) {
                return (float3)(0.0, 0.0, 0.0);
            }

            const float normal_diff =
                fmax(dot(normalize(light_dir + material->view_direction), material->normal), 0.0f);
            const float spec = pow(normal_diff, material->shininess);
            return strength * spec * material->specular_color;
        }

        float GetAttenuation(float3 light_pos, float3 settings, const LightingMaterialSettings* material) {
            const float distance = length(light_pos - material->frag_pos);
            return 1.0f / dot(settings, (float3)(1.0f, distance, distance * distance));
        }

        float GetShadow(const ShadowSettings* shadow_info, const LightingMaterialSettings* material) {
            const float kBias = 0.008f;
            const float* shadow_map = shadow_info->shadow_map;
            if (!material->shadow || !shadow_map) {
                return 0.0f;
            }
            const float4* shadow_space = shadow_info->shadow_space;
            const int2 shadow_size = shadow_info->shadow_size;

            float4 frag_pos = (float4)(material->frag_pos, 1.0f);
            frag_pos = (float4)(dot(shadow_space[0], frag_pos), dot(shadow_space[1], frag_pos),
                                dot(shadow_space[2], frag_pos), dot(shadow_space[3], frag_pos));
            frag_pos /= frag_pos.w;
            if (frag_pos.z > 1.0f) {
                return 0.0f;
            }

            float shadow = 0.0f;
            const float2 tex_coords = (float2)((frag_pos.x + 1.0f) / 2.0f, (frag_pos.y + 1.0) / 2.0f);
            const float2 texel_size = (float2)(1.0f / shadow_size.x, 1.0f / shadow_size.y);
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    const float2 ratio = tex_coords + (float2)(x, y) * texel_size;
                    const int2 coords = (int2)(ratio.x * shadow_size.x, ratio.y * shadow_size.y);
                    if (coords.x < 0 || shadow_size.x <= coords.x || coords.y < 0 || shadow_size.y <= coords.y) {
                        continue;
                    }
                    const float tex_depth = shadow_map[coords.x * shadow_size.y + coords.y];
                    if (frag_pos.z - kBias > tex_depth) {
                        shadow += 1.0f;
                    }
                }
            }

            return shadow / 9.0f;
        }
    );

    return Program("LightUtils", kLightUtilsSource);
}

}  // anonymous namespace

Program GetLightsProgram() {
    static constexpr std::string_view kLightsSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        float3 CalculateLighting(
            const LightDescription* light_desc, const LightingMaterialSettings* material_desc, const float* shadow_map
        ) {
            const int2 shadow_size = light_desc->shadow_size;
            ShadowSettings shadow_info = {
                .shadow_space = light_desc->shadow_space,
                .shadow_size = shadow_size,
                .shadow_map = shadow_size.x > 0 && shadow_size.y > 0 ? shadow_map : NULL,
            };

            switch (light_desc->light_type) {
                case LT_AMBIENT:
                    return CalculateAmbientLight(light_desc->strength.x, material_desc);
                case LT_DIRECT:
                    return CalculateDirectLight(
                        light_desc->direction, light_desc->strength, material_desc, &shadow_info
                    );
                case LT_POINT:
                    return CalculatePointLight(
                        light_desc->position, light_desc->strength, light_desc->attenuation, material_desc
                    );
                case LT_SPOT:
                    return CalculateSpotLight(
                        light_desc->position, light_desc->direction, light_desc->angle, light_desc->strength,
                        light_desc->attenuation, material_desc, &shadow_info
                    );
                default:
                    return (float3)(0.0f, 0.0f, 0.0f);
            }
        }
    );

    return ProgramBuilder("Lights", kLightsSource)
        .Include(GetVectorFunctionsProgram())
        .Include(GetLightUtilsProgram())
        .Include(AmbientLight::GetKernelProgram())
        .Include(DirectLight::GetKernelProgram())
        .Include(PointLight::GetKernelProgram())
        .Include(SpotLight::GetKernelProgram())
        .Build();
}

}  // namespace multithread::detail

AmbientLight::AmbientLight(FloatType strength)
    : strength_(strength) {
}

Vec3 AmbientLight::CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const {
    return strength_ * material.diffuse_color;
}

AmbientLight::Program AmbientLight::GetKernelProgram() {
    static constexpr std::string_view kAmbientLightSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        float3 CalculateAmbientLight(float strength, const LightingMaterialSettings* material) {
            return strength * material->diffuse_color;
        }
    );

    return Program("AmbientLight", kAmbientLightSource);
}

AmbientLight::LightDescription AmbientLight::GetDescription() const {
    return {
        .light_type = LightDescription::LT_AMBIENT,
        .strength = {.x = strength_},
    };
}

std::optional<AmbientLight::ShadowInfo> AmbientLight::GetShadowInfo() const {
    return std::nullopt;
}

void AmbientLight::ApplyTransform(const Transform& transform) {
}

DirectLight::DirectLight(Vec3 direction, const LightStrength& strength)
    : inversed_direction_(-direction.normalized())
    , strength_(strength) {
}

DirectLight& DirectLight::SetupShadow(ShadowSettings settings) {
    shadow_settings_ = settings;
    shadow_space_ = GetShadowSpaceTransform();

    const auto size = shadow_settings_->size;
    shadow_size_ = {
        .x = static_cast<cl_int>(size.x() / shadow_settings_->resolution),
        .y = static_cast<cl_int>(size.y() / shadow_settings_->resolution),
    };

    return *this;
}

Vec3 DirectLight::CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const {
    Vec3 color = strength_.ambient * material.diffuse_color;

    const auto normal_diff = inversed_direction_.dot(material.normal);
    if (normal_diff < 0.0) {
        return color;
    }

    const auto shadow = shadow_space_ ? 1.0 - GetShadow(*shadow_space_, depth, material) : 1.0;
    color += shadow * strength_.diffuse * normal_diff * material.diffuse_color;
    color += shadow * GetSpecularColor(inversed_direction_, strength_, material);

    return color;
}

DirectLight::Program DirectLight::GetKernelProgram() {
    static constexpr std::string_view kDirectLightSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        float3 CalculateDirectLight(
            float3 inversed_direction, float3 strength, const LightingMaterialSettings* material,
            const ShadowSettings* shadow_info
        ) {
            float3 color = strength.x * material->diffuse_color;

            const float normal_diff = dot(inversed_direction, material->normal);
            if (normal_diff < 0.0f) {
                return color;
            }

            const float shadow = 1.0f - GetShadow(shadow_info, material);
            color += shadow * strength.y * normal_diff * material->diffuse_color;
            color += shadow * GetSpecularColor(inversed_direction, strength.z, material);

            return color;
        }
    );

    return ProgramBuilder("DirectLight", kDirectLightSource).Include(GetLightUtilsProgram()).Build();
}

DirectLight::LightDescription DirectLight::GetDescription() const {
    LightDescription result = {
        .light_type = LightDescription::LT_DIRECT,
        .strength = GetClStrength(strength_),
        .direction = Vec3ToCl(inversed_direction_),
        .shadow_size = shadow_size_,
    };

    if (shadow_settings_) {
        TransformToCl(*shadow_space_, result.shadow_space);
    }

    return result;
}

std::optional<DirectLight::ShadowInfo> DirectLight::GetShadowInfo() const {
    if (!shadow_settings_) {
        return std::nullopt;
    }

    const auto position = shadow_settings_->position;
    return DirectLight::ShadowInfo{
        .shadow_width = static_cast<uint64_t>(shadow_size_.x),
        .shadow_height = static_cast<uint64_t>(shadow_size_.y),
        .transform = *shadow_space_,
        .light_pos = position
    };
}

VerticesObject DirectLight::VisualizeLight(Vec3 position, Vec4 color, FloatType scale) const {
    return VisualizeDirectedLight(position, -inversed_direction_, color, scale);
}

VerticesObject DirectLight::VisualizeShadowBox() const {
    assert(shadow_settings_ && "Can not create shadow visualization without settings");

    auto cube = CreateCube()
                    .ApplyTransform(Translation(0.0, 0.0, 0.5))
                    .ApplyTransform(Scale(shadow_settings_->size))
                    .ApplyTransform(GetOrientationTransform(-inversed_direction_))
                    .ApplyTransform(Translation(shadow_settings_->position))
                    .SetColors(Vec4(0.0, 0.0, 0.0, 0.7))
                    .SetMaterial({.shadow = false});

    return cube;
}

void DirectLight::ApplyTransform(const Transform& transform) {
    inversed_direction_ = (transform.linear() * inversed_direction_).normalized();
    if (shadow_settings_) {
        shadow_settings_->position = transform * shadow_settings_->position;
        shadow_space_ = GetShadowSpaceTransform();
    }
}

ProjectiveTransform DirectLight::GetShadowSpaceTransform() const {
    assert(shadow_settings_ && "Can not get shadow space transform without settings");

    const auto size = shadow_settings_->size;
    return ComposeCameraTransform(
        BoxProjection(size.x(), size.y(), size.z()), GetOrientationTransform(-inversed_direction_),
        shadow_settings_->position
    );
}

PointLight::PointLight(Vec3 position, const LightStrength& strength, const AttenuationSettings& attenuation)
    : position_(position)
    , strength_(strength)
    , attenuation_(attenuation) {
}

Vec3 PointLight::CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const {
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

PointLight::Program PointLight::GetKernelProgram() {
    static constexpr std::string_view kPointLightSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        float3 CalculatePointLight(
            float3 position, float3 strength, float3 attenuation_settings, const LightingMaterialSettings* material
        ) {
            const float attenuation = GetAttenuation(position, attenuation_settings, material);
            float3 color = strength.x * material->diffuse_color * attenuation;

            float3 light_dir = position - material->frag_pos;
            if (IsZeroFloat3(light_dir)) {
                return color;
            }
            light_dir = normalize(light_dir);

            const float normal_diff = dot(light_dir, material->normal);
            if (normal_diff < 0.0f) {
                return color;
            }

            color += strength.y * normal_diff * material->diffuse_color * attenuation;
            color += GetSpecularColor(light_dir, strength.z, material) * attenuation;

            return color;
        }
    );

    return ProgramBuilder("PointLight", kPointLightSource).Include(GetLightUtilsProgram()).Build();
}

PointLight::LightDescription PointLight::GetDescription() const {
    return {
        .light_type = LightDescription::LT_POINT,
        .strength = GetClStrength(strength_),
        .attenuation = GetClAttenuation(attenuation_),
        .position = Vec3ToCl(position_)
    };
}

std::optional<PointLight::ShadowInfo> PointLight::GetShadowInfo() const {
    return std::nullopt;
}

VerticesObject PointLight::VisualizeLight(Vec4 color, FloatType scale) const {
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
    , light_angle_(settings.light_angle * settings.light_angle_ratio)
    , cut_in_(std::cos(settings.light_angle / 2.0))
    , cut_out_(std::cos(settings.light_angle * settings.light_angle_ratio / 2.0)) {
    assert(Less(0.0, settings.light_angle) && "Spot light angle shuld be at least zero");
    assert(Less(1.0, settings.light_angle_ratio) && "Spot light angle ratio should be at least one");

    const auto max_angle = std::numbers::pi;
    assert(Less(settings.light_angle, max_angle) && "Spot light angle shuld be at most pi");
    assert(Less(settings.light_angle * settings.light_angle_ratio, max_angle) && "Spot light angle ratio too large");
}

SpotLight& SpotLight::SetupShadow(ShadowSettings settings) {
    shadow_settings_ = settings;
    shadow_space_ = GetShadowSpaceTransform();

    const cl_int size =
        2.0 * std::tan(light_angle_ / 2) * shadow_settings_->max_distance / shadow_settings_->resolution;
    shadow_size_ = {.x = size, .y = size};

    return *this;
}

Vec3 SpotLight::CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const {
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

    const auto shadow = shadow_space_ ? 1.0 - GetShadow(*shadow_space_, depth, material) : 1.0;
    color += shadow * strength_.diffuse * normal_diff * material.diffuse_color * attenuation;
    color += shadow * GetSpecularColor(light_dir, strength_, material) * attenuation;

    return color;
}

SpotLight::Program SpotLight::GetKernelProgram() {
    static constexpr std::string_view kSpotLightSource = BOOST_COMPUTE_STRINGIZE_SOURCE(

        float3 CalculateSpotLight(
            float3 position, float3 inversed_direction, float2 angle, float3 strength, float3 attenuation_settings,
            const LightingMaterialSettings* material, const ShadowSettings* shadow_info
        ) {
            float attenuation = GetAttenuation(position, attenuation_settings, material);
            float3 color = strength.x * material->diffuse_color * attenuation;

            float3 light_dir = position - material->frag_pos;
            if (IsZeroFloat3(light_dir)) {
                return color;
            }
            light_dir = normalize(light_dir);

            const float normal_diff = dot(light_dir, material->normal);
            if (normal_diff < 0.0f) {
                return color;
            }

            const float theta = dot(light_dir, inversed_direction);
            attenuation *= clamp((theta - angle.y) / (angle.x - angle.y), 0.0f, 1.0f);

            const float shadow = 1.0f - GetShadow(shadow_info, material);
            color += shadow * strength.y * normal_diff * material->diffuse_color * attenuation;
            color += shadow * GetSpecularColor(light_dir, strength.z, material) * attenuation;

            return color;
        }
    );

    return ProgramBuilder("SpotLight", kSpotLightSource).Include(GetLightUtilsProgram()).Build();
}

SpotLight::LightDescription SpotLight::GetDescription() const {
    LightDescription result = {
        .light_type = LightDescription::LT_SPOT,
        .strength = GetClStrength(strength_),
        .attenuation = GetClAttenuation(attenuation_),
        .position = Vec3ToCl(position_),
        .direction = Vec3ToCl(inversed_direction_),
        .angle = {.x = cut_in_, .y = cut_out_},
        .shadow_size = shadow_size_,
    };

    if (shadow_settings_) {
        TransformToCl(*shadow_space_, result.shadow_space);
    }

    return result;
}

std::optional<SpotLight::ShadowInfo> SpotLight::GetShadowInfo() const {
    if (!shadow_settings_) {
        return std::nullopt;
    }

    return SpotLight::ShadowInfo{
        .shadow_width = static_cast<uint64_t>(shadow_size_.x),
        .shadow_height = static_cast<uint64_t>(shadow_size_.y),
        .transform = *shadow_space_,
        .light_pos = position_,
    };
}

VerticesObject SpotLight::VisualizeLight(Vec4 color, FloatType scale) const {
    return VisualizeDirectedLight(position_, -inversed_direction_, color, scale);
}

VerticesObject SpotLight::VisualizeShadowBox() const {
    assert(shadow_settings_ && "Can not create shadow visualization without settings");

    auto quad_face = CreateQuad(true).ApplyTransform(Scale(2.0)).ApplyTransform(Translation(0.0, 0.0, 1.0));
    auto result = quad_face;
    result.GenerateNormals(false);

    const auto delt = shadow_settings_->min_distance / shadow_settings_->max_distance;
    result.Merge(quad_face.ApplyTransform(Scale(delt)).GenerateNormals());

    const std::vector<Vec3> positions = {
        Vec3(1.0, -1.0, 1.0),
        Vec3(1.0, 1.0, 1.0),
        Vec3(delt, delt, delt),
        Vec3(delt, -delt, delt),
    };
    const std::vector<uint64_t> indices = {0, 1, 2, 2, 3, 0};
    auto side_face = VerticesObject(4, VerticesObject::Type::Triangles)
                         .SetPositions(positions)
                         .SetIndices(indices)
                         .GenerateNormals(false);
    result.Merge(side_face);

    const auto z_axis = Vec3(0.0, 0.0, 1.0);
    const auto z_rotation = Rotation(z_axis, std::numbers::pi / 2.0);
    result.Merge(side_face.ApplyTransform(z_rotation));
    result.Merge(side_face.ApplyTransform(z_rotation));
    result.Merge(side_face.ApplyTransform(z_rotation));

    const auto angle = std::tan(light_angle_ / 2);
    result.ApplyTransform(Scale(shadow_settings_->max_distance * Vec3(angle, angle, 1.0)))
        .ApplyTransform(GetOrientationTransform(-inversed_direction_))
        .ApplyTransform(Translation(position_))
        .SetColors(Vec4(0.0, 0.0, 0.0, 0.7))
        .SetMaterial({.shadow = false});

    return result;
}

void SpotLight::ApplyTransform(const Transform& transform) {
    inversed_direction_ = (transform.linear() * inversed_direction_).normalized();
    position_ = transform * position_;
    if (shadow_settings_) {
        shadow_space_ = GetShadowSpaceTransform();
    }
}

ProjectiveTransform SpotLight::GetShadowSpaceTransform() const {
    assert(shadow_settings_ && "Can not get shadow space transform without settings");

    return ComposeCameraTransform(
        PerspectiveProjection(light_angle_, 1.0, shadow_settings_->min_distance, shadow_settings_->max_distance),
        GetOrientationTransform(-inversed_direction_), position_
    );
}

}  // namespace null_engine
