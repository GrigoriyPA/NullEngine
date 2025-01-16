#pragma once

#include <array>

#include "vector4.hpp"

namespace null_engine {

class Mat4 {
public:
    static constexpr uint32_t kSize = 4;

    Mat4();

    Mat4(std::initializer_list<std::initializer_list<FloatType>> init);

    Mat4& operator*=(const Mat4& other);

    friend Mat4 operator*(const Mat4& left, const Mat4& right);

    FloatType GetElement(uint32_t i, uint32_t j) const;

    Mat4& Transpose();

    static Mat4 Transpose(const Mat4& other);

    Vec3 Apply(Vec3 vector) const;

    Vec4 Apply(Vec4 vector) const;

    static Mat4 Scale(Vec3 scale);

    static Mat4 Scale(FloatType scale_x, FloatType scale_y, FloatType scale_z);

    static Mat4 Scale(FloatType scale);

    static Mat4 Translation(Vec3 translation);

    static Mat4 Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z);

    static Mat4 Rotation(Vec3 axis, FloatType angle);

    static Mat4 Basis(Vec3 x, Vec3 y, Vec3 z);

    static Mat4 BoxProjection(FloatType width, FloatType height, FloatType depth);

    static Mat4 PerspectiveProjection(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance);

private:
    void Fill(FloatType valie);

    std::array<std::array<FloatType, kSize>, kSize> matrix_;
};

std::ostream& operator<<(std::ostream& out, const Mat4& transform);

}  // namespace null_engine
