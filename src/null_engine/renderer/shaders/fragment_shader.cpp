#include "fragment_shader.hpp"

namespace null_engine::detail {

Vec3 FragmentShader::GetViewPos() const {
    return view_pos_;
}

Vec3 FragmentShader::GetPointColor(const InterpolationParams& params) const {
    if (material_.diffuse_tex.HasTexture()) {
        return material_.diffuse_tex.GetColor(params.tex_coords);
    }
    return params.color;
}

void FragmentShader::SetViewPos(Vec3 view_pos) {
    view_pos_ = view_pos;
}

void FragmentShader::SetMaterial(const Material& material) {
    material_ = material;
}

}  // namespace null_engine::detail
