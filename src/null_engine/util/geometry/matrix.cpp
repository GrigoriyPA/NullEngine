#include "matrix.hpp"

#include "helpers.hpp"

namespace null_engine {

Transform NormalTransform(const Transform& transform) {
    return Transform(transform.linear().inverse().transpose());
}

Transform Ident() {
    return Transform(Mat3({
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
    }));
}

Transform Scale(Vec3 scale) {
    return Transform(Mat3({
        {scale.x(), 0.0, 0.0},
        {0.0, scale.y(), 0.0},
        {0.0, 0.0, scale.z()},
    }));
}

Transform Scale(FloatType scale_x, FloatType scale_y, FloatType scale_z) {
    return Scale(Vec3(scale_x, scale_y, scale_z));
}

Transform Scale(FloatType scale) {
    return Scale(scale, scale, scale);
}

Transform Translation(Vec3 translation) {
    return Transform(Mat4({
        {1.0, 0.0, 0.0, translation.x()},
        {0.0, 1.0, 0.0, translation.y()},
        {0.0, 0.0, 1.0, translation.z()},
        {0.0, 0.0, 0.0, 1.0},
    }));
}

Transform Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z) {
    return Translation(Vec3(translation_x, translation_y, translation_z));
}

Transform Rotation(Vec3 axis, FloatType angle) {
    axis.normalize();

    const FloatType x = axis.x();
    const FloatType y = axis.y();
    const FloatType z = axis.z();
    const FloatType c = cos(angle);
    const FloatType ic = 1.0 - c;
    const FloatType s = sin(angle);

    return Transform(Mat3({
        {c + x * x * ic, x * y * ic - z * s, x * z * ic + y * s},
        {y * x * ic + z * s, c + y * y * ic, y * z * ic - x * s},
        {z * x * ic - y * s, z * y * ic + x * s, c + z * z * ic},
    }));
}

Transform Basis(Vec3 x, Vec3 y, Vec3 z) {
    return Transform(Mat3({
        {x.x(), y.x(), z.x()},
        {x.y(), y.y(), z.y()},
        {x.z(), y.z(), z.z()},
    }));
}

ProjectiveTransform BoxProjection(FloatType width, FloatType height, FloatType depth) {
    return Translation(0.0, 0.0, -1.0) * Scale(2.0 / width, 2.0 / height, 2.0 / depth);
}

ProjectiveTransform PerspectiveProjection(
    FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance
) {
    assert(Less(0.0, min_distance) && "Min projection distance should be positive");
    assert(Less(min_distance, max_distance) && "Min and max projection distances invalid");

    const FloatType t = tan(fov / 2.0);
    assert(Less(0.0, t) && "Invalid fov, value should be in interval (0; PI)");

    ProjectiveTransform transform(
        Scale(1.0 / t, ratio / t, (max_distance + min_distance) / (max_distance - min_distance)) *
        Translation(0.0, 0.0, -2.0 * max_distance * min_distance / (max_distance + min_distance))
    );

    transform(3, 3) = 0.0;
    transform(3, 2) = 1.0;

    return transform;
}

}  // namespace null_engine
