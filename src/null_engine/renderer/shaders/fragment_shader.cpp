#include "fragment_shader.hpp"

namespace null_engine::detail {

Vec3 FragmentShader::GetViewPos() const {
    return view_pos_;
}

void FragmentShader::SetViewPos(Vec3 view_pos) {
    view_pos_ = view_pos;
}

std::optional<Vec3> FragmentShader::GetPointColor(const InterpolationParams& params) const {
    if (!params.normal.IsZero()) {
        if (params.normal.ScalarProd(view_pos_ - params.frag_pos) < 0.0) {
            return std::nullopt;
        }
    }

    return params.color;
}

}  // namespace null_engine::detail
