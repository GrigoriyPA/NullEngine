#include "light.hpp"

namespace null_engine {

AmbientLight::AmbientLight(FloatType strength)
    : strength_(strength) {
}

Vec3 AmbientLight::CalculateLighting(const LightingMaterialSettings& settings) const {
    return strength_ * settings.diffuse_color;
}

DirectLight::DirectLight(const Vec3& direction, const LightStrength& strength)
    : inversed_direction_(-Vec3::Normalize(direction))
    , strength_(strength) {
}

Vec3 DirectLight::CalculateLighting(const LightingMaterialSettings& settings) const {
    Vec3 color = strength_.ambient * settings.diffuse_color;

    const auto normal_diff = inversed_direction_.ScalarProd(settings.normal);
    if (normal_diff < 0.0) {
        return color;
    }
    color += strength_.diffuse * normal_diff * settings.diffuse_color;

    const Vec3 halfway_dir = (inversed_direction_ + settings.view_direction).Normalize();
    const auto spec = std::pow(std::max(halfway_dir.ScalarProd(settings.normal), 0.0), settings.shininess);
    color += strength_.specular * spec * settings.specular_color;

    return color;
}

VerticesObject DirectLight::VisualizeLight(Vec3 position, Vec3 color, FloatType scale) const {
    const uint64_t number_vertices = 6;
    VerticesObject result(number_vertices, VerticesObject::Type::Lines);

    const auto direction = -inversed_direction_;
    const auto horizon = direction.Horizon();
    const auto vertical = horizon.VectorProd(direction);

    result.SetPositions(
        {horizon + vertical, -horizon - vertical, -horizon + vertical, horizon - vertical, Vec3(), 2.0 * direction}
    );
    result.SetParams(VertexParams{.color = color});
    result.Transform(Mat4::Translation(position) * Mat4::Scale(scale));

    return result;
}

}  // namespace null_engine
