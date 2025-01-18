#pragma once

#include <optional>

#include "vertex_shader.hpp"

namespace null_engine::detail {

class FragmentShader {
public:
    Vec3 GetViewPos() const;

    void SetViewPos(Vec3 view_pos);

    std::optional<Vec3> GetPointColor(const InterpolationParams& params) const;

private:
    Vec3 view_pos_;
};

}  // namespace null_engine::detail
