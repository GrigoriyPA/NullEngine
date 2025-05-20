#include "vector.hpp"

#include <boost/compute/utility/source.hpp>
#include <string_view>

#include "helpers.hpp"

namespace null_engine {

float OrientedArea(Vec2 left, Vec2 right) {
    return left.y() * right.x() - left.x() * right.y();
}

float OrientedArea(Vec2 point_a, Vec2 point_b, Vec2 point_c) {
    return OrientedArea(point_b - point_a, point_c - point_a);
}

Vec3 VectorProd(Vec3 left, Vec3 right) {
    return Vec3(
        left.z() * right.y() - left.y() * right.z(), left.x() * right.z() - left.z() * right.x(),
        left.y() * right.x() - left.x() * right.y()
    );
}

Vec3 Horizon(Vec3 vector) {
    if (Equal(vector.x(), 0.0) && Equal(vector.z(), 0.0)) {
        return Vec3(1.0, 0.0, 0.0);
    }

    return Vec3(vector.z(), 0.0, -vector.x());
}

Vec3 Vec4ToVec3(Vec4 vector) {
    return Vec3(vector.x(), vector.y(), vector.z());
}

Vec4 Vec3ToVec4(Vec3 vector, float w) {
    return Vec4(vector.x(), vector.y(), vector.z(), w);
}

namespace multithread::detail {

Program GetVectorFunctionsProgram() {
    static constexpr std::string_view kVectorFunctionsSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        const float kEps = 1e-6;

        float VectorProd2d(float2 left, float2 right) { return left.y * right.x - left.x * right.y; }

        float OrientedArea(float2 point_a, float2 point_b, float2 point_c) {
            return VectorProd2d(point_b - point_a, point_c - point_a);
        }

        bool IsZeroFloat3(float3 v) { return fabs(v.x) < kEps && fabs(v.y) < kEps && fabs(v.z) < kEps; }
    );

    return Program("VectorFunctions", kVectorFunctionsSource);
}

}  // namespace multithread::detail

}  // namespace null_engine
