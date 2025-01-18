#include "vertex_shader.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine::detail {

InterpolationParams& InterpolationParams::operator+=(const InterpolationParams& other) {
    color += other.color;
    normal += other.normal;
    frag_pos += other.frag_pos;
    return *this;
}

InterpolationParams& InterpolationParams::operator-=(const InterpolationParams& other) {
    color -= other.color;
    normal -= other.normal;
    frag_pos -= other.frag_pos;
    return *this;
}

InterpolationParams& InterpolationParams::operator*=(FloatType scale) {
    color *= scale;
    normal *= scale;
    frag_pos *= scale;
    return *this;
}

InterpolationParams& InterpolationParams::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    color /= scale;
    normal /= scale;
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
    const Mat4& camera_transform, const Mat4& object_transform, const std::vector<Vertex>& verices
) {
    const auto ndc_transform = camera_transform * object_transform;
    const auto normal_transform = Mat3::Inverse(Mat4::ToMat3(object_transform)).Transpose();

    std::vector<InterpVertex> result;
    result.reserve(verices.size());
    for (const auto& [position, params] : verices) {
        result.emplace_back(
            ndc_transform.Apply(position),
            InterpolationParams{
                .color = params.color,
                .normal = normal_transform.Apply(params.normal),
                .frag_pos = object_transform.Apply(position).XYZ()
            }
        );
    }
    return result;
}

}  // namespace null_engine::detail
