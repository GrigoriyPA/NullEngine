#pragma once

#include <null_engine/acceleration/program.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>

#include "light_interface.hpp"

namespace null_engine {

class AmbientLight {
    using LightDescription = ILight::LightDescription;
    using ShadowInfo = ILight::ShadowInfo;
    using DepthBuffer = ILight::DepthBuffer;
    using Program = multithread::detail::Program;

public:
    explicit AmbientLight(FloatType strength);

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    FloatType strength_;
};

struct LightStrength {
    FloatType ambient;
    FloatType diffuse;
    FloatType specular;
};

class DirectLight {
    using LightDescription = ILight::LightDescription;
    using ShadowInfo = ILight::ShadowInfo;
    using DepthBuffer = ILight::DepthBuffer;
    using Program = multithread::detail::Program;

public:
    struct ShadowSettings {
        Vec3 position;
        Vec3 size;
        FloatType resolution;
    };

    DirectLight(Vec3 direction, const LightStrength& strength);

    DirectLight& SetupShadow(ShadowSettings settings);

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    VerticesObject VisualizeLight(Vec3 position, Vec3 color = kWhite, FloatType scale = 1.0) const;

    VerticesObject VisualizeShadowBox() const;

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    ProjectiveTransform GetShadowSpaceTransform() const;

    Vec3 inversed_direction_;
    LightStrength strength_;
    std::optional<ShadowSettings> shadow_settings_;
    std::optional<ProjectiveTransform> shadow_space_;
};

struct AttenuationSettings {
    FloatType constant = 1.0;
    FloatType linear = 0.0;
    FloatType quadratic = 0.0;
};

class PointLight {
    using LightDescription = ILight::LightDescription;
    using ShadowInfo = ILight::ShadowInfo;
    using DepthBuffer = ILight::DepthBuffer;
    using Program = multithread::detail::Program;

public:
    PointLight(Vec3 position, const LightStrength& strength, const AttenuationSettings& attenuation = {});

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    VerticesObject VisualizeLight(Vec3 color = kWhite, FloatType scale = 1.0) const;

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    Vec3 position_;
    LightStrength strength_;
    AttenuationSettings attenuation_;
};

class SpotLight {
    using LightDescription = ILight::LightDescription;
    using ShadowInfo = ILight::ShadowInfo;
    using DepthBuffer = ILight::DepthBuffer;
    using Program = multithread::detail::Program;

public:
    struct Settings {
        Vec3 position = Vec3(0.0, 0.0, 0.0);
        Vec3 direction = Vec3(0.0, 0.0, 1.0);
        FloatType light_angle;
        FloatType light_angle_ratio = 1.1;
    };

    struct ShadowSettings {
        FloatType min_distance;
        FloatType max_distance;
        FloatType resolution;
    };

    SpotLight(const Settings& settings, const LightStrength& strength, const AttenuationSettings& attenuation = {});

    SpotLight& SetupShadow(ShadowSettings settings);

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    VerticesObject VisualizeLight(Vec3 color = kWhite, FloatType scale = 1.0) const;

    // VerticesObject VisualizeShadowBox() const; TODO

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    ProjectiveTransform GetShadowSpaceTransform() const;

    Vec3 position_;
    Vec3 inversed_direction_;
    FloatType light_angle_;
    FloatType cut_in_;
    FloatType cut_out_;
    LightStrength strength_;
    AttenuationSettings attenuation_;
    std::optional<ShadowSettings> shadow_settings_;
    std::optional<ProjectiveTransform> shadow_space_;
};

namespace multithread::detail {

Program GetLightsProgram();

}  // namespace multithread::detail

}  // namespace null_engine
