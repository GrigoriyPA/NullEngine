#pragma once

#include <null_engine/drawable_objects/common/vertex.hpp>
#include <null_engine/util/geometry/matrix.hpp>
#include <vector>

namespace null_engine::detail {

class InterpolationParams {
public:
    Vec3 color = Vec3(0.0, 0.0, 0.0);
    Vec3 normal = Vec3(0.0, 0.0, 0.0);
    Vec2 tex_coords = Vec2(0.0, 0.0);
    Vec3 frag_pos = Vec3(0.0, 0.0, 0.0);

    InterpolationParams& operator+=(const InterpolationParams& other);

    InterpolationParams& operator-=(const InterpolationParams& other);

    InterpolationParams& operator*=(FloatType scale);

    InterpolationParams& operator/=(FloatType scale);
};

class InterpVertex {
public:
    Vec4 position = Vec4(0.0, 0.0, 0.0, 0.0);
    InterpolationParams params;

    InterpVertex& operator+=(const InterpVertex& other);

    InterpVertex& operator-=(const InterpVertex& other);

    InterpVertex& operator*=(FloatType scale);

    InterpVertex& operator/=(FloatType scale);
};

std::vector<InterpVertex> ConvertObjectVerices(
    const ProjectiveTransform& camera_transform, const Transform& object_transform, const std::vector<Vertex>& verices
);

}  // namespace null_engine::detail
