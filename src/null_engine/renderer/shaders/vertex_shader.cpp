#include "vertex_shader.hpp"

#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine::detail {

InterpolationParams& InterpolationParams::operator+=(const InterpolationParams& other) {
    color += other.color;
    normal += other.normal;
    tex_coords += other.tex_coords;
    frag_pos += other.frag_pos;
    return *this;
}

InterpolationParams& InterpolationParams::operator-=(const InterpolationParams& other) {
    color -= other.color;
    normal -= other.normal;
    tex_coords -= other.tex_coords;
    frag_pos -= other.frag_pos;
    return *this;
}

InterpolationParams& InterpolationParams::operator*=(FloatType scale) {
    color *= scale;
    normal *= scale;
    tex_coords *= scale;
    frag_pos *= scale;
    return *this;
}

InterpolationParams& InterpolationParams::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    color /= scale;
    normal /= scale;
    tex_coords /= scale;
    frag_pos /= scale;
    return *this;
}

InterpVertex& InterpVertex::operator+=(const InterpVertex& other) {
    position += other.position;
    params += other.params;
    return *this;
}

InterpVertex& InterpVertex::operator-=(const InterpVertex& other) {
    position -= other.position;
    params -= other.params;
    return *this;
}

InterpVertex& InterpVertex::operator*=(FloatType scale) {
    position *= scale;
    params *= scale;
    return *this;
}

InterpVertex& InterpVertex::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    position /= scale;
    params /= scale;
    return *this;
}

std::vector<InterpVertex> ConvertObjectVerices(
    const ProjectiveTransform& camera_transform, const Transform& object_transform, const std::vector<Vertex>& verices
) {
    const auto ndc_transform = camera_transform * object_transform;
    const auto normal_transform = NormalTransform(object_transform);

    std::vector<InterpVertex> result;
    result.reserve(verices.size());
    for (const auto& [position, params] : verices) {
        result.emplace_back(
            ndc_transform * Vec4(position.x(), position.y(), position.z(), 1.0),
            InterpolationParams{
                .color = params.color,
                .normal = normal_transform * params.normal,
                .tex_coords = params.tex_coords,
                .frag_pos = object_transform * position
            }
        );
    }
    return result;
}

}  // namespace null_engine::detail
