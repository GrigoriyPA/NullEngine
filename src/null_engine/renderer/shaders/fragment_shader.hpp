#pragma once

#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/scene/lights/light_interface.hpp>

#include "vertex_shader.hpp"

namespace null_engine::detail {

class FragmentShader {
public:
    static constexpr uint8_t kMaxNumberLights = 1;

    Vec3 GetViewPos() const;

    Vec3 GetPointColor(const InterpolationParams& params) const;

    void SetViewPos(Vec3 view_pos);

    void SetMaterial(const Material& material);

    void SetLights(const std::vector<AnyLight>& lights);

private:
    Vec3 view_pos_;
    Material material_;
    uint32_t number_lights_;
    std::array<AnyLight, kMaxNumberLights> lights_;
};

}  // namespace null_engine::detail
