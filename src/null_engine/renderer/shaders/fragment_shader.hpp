#pragma once

#include <null_engine/drawable_objects/material/material.hpp>

#include "vertex_shader.hpp"

namespace null_engine::detail {

class FragmentShader {
public:
    Vec3 GetViewPos() const;

    Vec3 GetPointColor(const InterpolationParams& params) const;

    void SetViewPos(Vec3 view_pos);

    void SetMaterial(const Material& material);

private:
    Vec3 view_pos_;
    Material material_;
};

}  // namespace null_engine::detail
