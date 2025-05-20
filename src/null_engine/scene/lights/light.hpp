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
    explicit AmbientLight(float strength);

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    float strength_;
};

struct LightStrength {
    float ambient;
    float diffuse;
    float specular;
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
        float resolution;
    };

    DirectLight(Vec3 direction, const LightStrength& strength);

    DirectLight& SetupShadow(ShadowSettings settings);

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    VerticesObject VisualizeLight(Vec3 position, Vec4 color = kWhite, float scale = 1.0) const;

    VerticesObject VisualizeShadowBox() const;

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    ProjectiveTransform GetShadowSpaceTransform() const;

    Vec3 inversed_direction_;
    LightStrength strength_;
    cl_int2 shadow_size_ = {0, 0};
    std::optional<ShadowSettings> shadow_settings_;
    std::optional<ProjectiveTransform> shadow_space_;
};

struct AttenuationSettings {
    float constant = 1.0;
    float linear = 0.0;
    float quadratic = 0.0;
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

    VerticesObject VisualizeLight(Vec4 color = kWhite, float scale = 1.0) const;

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
        float light_angle;
        float light_angle_ratio = 1.1;
    };

    struct ShadowSettings {
        float min_distance;
        float max_distance;
        float resolution;
    };

    SpotLight(const Settings& settings, const LightStrength& strength, const AttenuationSettings& attenuation = {});

    SpotLight& SetupShadow(ShadowSettings settings);

    Vec3 CalculateLighting(const LightingMaterialSettings& material, DepthBuffer depth) const;

    LightDescription GetDescription() const;

    std::optional<ShadowInfo> GetShadowInfo() const;

    VerticesObject VisualizeLight(Vec4 color = kWhite, float scale = 1.0) const;

    VerticesObject VisualizeShadowBox() const;

    void ApplyTransform(const Transform& transform);

    static Program GetKernelProgram();

private:
    ProjectiveTransform GetShadowSpaceTransform() const;

    Vec3 position_;
    Vec3 inversed_direction_;
    float light_angle_;
    float cut_in_;
    float cut_out_;
    LightStrength strength_;
    AttenuationSettings attenuation_;
    cl_int2 shadow_size_ = {0, 0};
    std::optional<ShadowSettings> shadow_settings_;
    std::optional<ProjectiveTransform> shadow_space_;
};

namespace multithread::detail {

Program GetLightsProgram();

}  // namespace multithread::detail

}  // namespace null_engine
