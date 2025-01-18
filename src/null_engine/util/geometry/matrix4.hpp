#pragma once

#include "matrix3.hpp"
#include "vector4.hpp"

namespace null_engine {

class Mat4 {
public:
    static constexpr uint32_t kSize = 4;

    Mat4();

    Mat4(std::initializer_list<std::initializer_list<FloatType>> init);

    Mat4(std::initializer_list<Vec4> rows);

    Mat4& operator*=(const Mat4& other);

    friend Mat4 operator*(const Mat4& left, const Mat4& right);

    Mat4& operator*=(FloatType scale);

    friend Mat4 operator*(Mat4 matrix, FloatType scale);

    Mat4& operator/=(FloatType scale);

    friend Mat4 operator/(Mat4 matrix, FloatType scale);

    FloatType GetElement(uint32_t i, uint32_t j) const;

    Vec4 GetRow(uint32_t i) const;

    Vec4 GetColumn(uint32_t j) const;

    Vec4 Apply(Vec3 vector) const;

    Vec4 Apply(Vec4 vector) const;

    Mat4& Fill(FloatType valie);

    Mat4& SetRow(uint32_t i, Vec4 row);

    Mat4& SetColumn(uint32_t j, Vec4 column);

    Mat4& Transpose();

    static Mat4 Transpose(const Mat4& other);

    static Mat3 ToMat3(const Mat4& tranform);

    static Mat4 FromMat3(const Mat3& tranform);

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
    std::array<std::array<FloatType, kSize>, kSize> matrix_;
};

Mat4 operator*(FloatType scale, const Mat4& matrix);

std::ostream& operator<<(std::ostream& out, const Mat4& transform);

}  // namespace null_engine
