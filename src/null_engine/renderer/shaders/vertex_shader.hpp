#pragma once

#include <null_engine/drawable_objects/common/vertex.hpp>
#include <null_engine/util/geometry/matrix4.hpp>
#include <vector>

namespace null_engine::detail {

class InterpolationParams {
public:
    Vec3 color;
    Vec3 normal;
    Vec2 tex_coords;
    Vec3 frag_pos;

    InterpolationParams& operator+=(const InterpolationParams& other);

    InterpolationParams& operator-=(const InterpolationParams& other);

    InterpolationParams& operator*=(FloatType scale);

    InterpolationParams& operator/=(FloatType scale);
};

class InterpVertex {
public:
    Vec4 position;
    InterpolationParams params;

    InterpVertex& operator+=(const InterpVertex& other);

    InterpVertex& operator-=(const InterpVertex& other);

    InterpVertex& operator*=(FloatType scale);

    InterpVertex& operator/=(FloatType scale);
};

std::vector<InterpVertex> ConvertObjectVerices(
    const Mat4& camera_transform, const Mat4& object_transform, const std::vector<Vertex>& verices
);

}  // namespace null_engine::detail
