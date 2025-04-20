#pragma once

#include <array>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/scene/lights/light_interface.hpp>

#include "vertex_shader.hpp"

namespace null_engine::native::detail {

class NoopFragmentShader {
    using InterpolationParams = null_engine::detail::InterpolationParams;

public:
    Vec3 GetPointColor(const InterpolationParams& params) const;
};

struct LightSettings {
    AnyLight light;
    ILight::DepthBuffer depth;
};

class MainFragmentShader {
    using InterpolationParams = null_engine::detail::InterpolationParams;

public:
    static constexpr uint32_t kMaxNumberLights = 1;

    Vec3 GetViewPos() const;

    Vec3 GetPointColor(const InterpolationParams& params) const;

    void SetViewPos(Vec3 view_pos);

    void SetMaterial(const Material& material);

    void SetLights(const std::vector<LightSettings>& lights);

private:
    Vec3 view_pos_ = Vec3(0.0, 0.0, 0.0);
    Material material_;
    uint32_t number_lights_;
    std::array<LightSettings, kMaxNumberLights> lights_;
};

}  // namespace null_engine::native::detail
