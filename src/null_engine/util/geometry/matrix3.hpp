#pragma once

#include <array>

#include "vector3.hpp"

namespace null_engine {

class Mat3 {
public:
    static constexpr uint32_t kSize = 3;

    Mat3();

    Mat3(std::initializer_list<std::initializer_list<FloatType>> init);

    Mat3(std::initializer_list<Vec3> rows);

    Mat3& operator*=(const Mat3& other);

    friend Mat3 operator*(const Mat3& left, const Mat3& right);

    Mat3& operator*=(FloatType scale);

    friend Mat3 operator*(Mat3 matrix, FloatType scale);

    Mat3& operator/=(FloatType scale);

    friend Mat3 operator/(Mat3 matrix, FloatType scale);

    FloatType GetElement(uint32_t i, uint32_t j) const;

    Vec3 GetRow(uint32_t i) const;

    Vec3 GetColumn(uint32_t j) const;

    FloatType AlgebraicAddition(uint32_t i, uint32_t j) const;

    Vec3 Apply(Vec2 vector) const;

    Vec3 Apply(Vec3 vector) const;

    Mat3& Fill(FloatType valie);

    Mat3& SetRow(uint32_t i, Vec3 row);

    Mat3& SetColumn(uint32_t j, Vec3 column);

    Mat3& Transpose();

    static Mat3 Transpose(const Mat3& other);

    Mat3& Inverse();

    static Mat3 Inverse(const Mat3& other);

    static Mat3 AttachedMatrix(const Mat3& other);

    static Mat3 Scale(Vec2 scale);

    static Mat3 Scale(FloatType scale_x, FloatType scale_y);

    static Mat3 Scale(FloatType scale);

    static Mat3 Translation(Vec2 translation);

    static Mat3 Translation(FloatType translation_x, FloatType translation_y);

    static Mat3 Rotation(Vec2 point, FloatType angle);

    static Mat3 Basis(Vec2 x, Vec2 y);

private:
    std::array<std::array<FloatType, kSize>, kSize> matrix_;
};

Mat3 operator*(FloatType scale, const Mat3& matrix);

std::ostream& operator<<(std::ostream& out, const Mat3& transform);

}  // namespace null_engine
