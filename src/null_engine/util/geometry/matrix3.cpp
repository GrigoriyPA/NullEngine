#include "matrix3.hpp"

#include <cassert>

#include "helpers.hpp"

namespace null_engine {

Mat3::Mat3() {
    Fill(0.0);

    for (uint32_t i = 0; i < kSize; ++i) {
        matrix_[i][i] = 1.0;
    }
}

Mat3::Mat3(std::initializer_list<std::initializer_list<FloatType>> init) {
    assert(init.size() == kSize && "Invalid initializer list size for transform");

    for (uint32_t i = 0; const auto& init_row : init) {
        assert(init_row.size() == kSize && "Invalid initializer list row size for transform");

        for (uint32_t j = 0; FloatType init_element : init_row) {
            matrix_[i][j++] = init_element;
        }
        ++i;
    }
}

Mat3::Mat3(std::initializer_list<Vec3> rows) {
    assert(rows.size() == kSize && "Invalid initializer list size for transform");

    for (uint32_t i = 0; const auto& init_row : rows) {
        SetRow(i++, init_row);
    }
}

Mat3& Mat3::operator*=(const Mat3& other) {
    return *this = *this * other;
}

Mat3 operator*(const Mat3& left, const Mat3& right) {
    Mat3 result;
    for (uint32_t i = 0; i < Mat3::kSize; ++i) {
        for (uint32_t j = 0; j < Mat3::kSize; ++j) {
            result.matrix_[i][j] = 0.0;

            for (uint32_t k = 0; k < Mat3::kSize; ++k) {
                result.matrix_[i][j] += left.matrix_[i][k] * right.matrix_[k][j];
            }
        }
    }
    return result;
}

Mat3& Mat3::operator*=(FloatType scale) {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] *= scale;
        }
    }
    return *this;
}

Mat3 operator*(Mat3 matrix, FloatType scale) {
    matrix *= scale;
    return matrix;
}

Mat3& Mat3::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] /= scale;
        }
    }
    return *this;
}

Mat3 operator/(Mat3 matrix, FloatType scale) {
    matrix /= scale;
    return matrix;
}

FloatType Mat3::GetElement(uint32_t i, uint32_t j) const {
    assert(i < kSize && j < kSize && "Mat3 element index too large");

    return matrix_[i][j];
}

Vec3 Mat3::GetRow(uint32_t i) const {
    assert(i < kSize && "Mat3 row index too large");
    return Vec3(matrix_[i][0], matrix_[i][1], matrix_[i][2]);
}

Vec3 Mat3::GetColumn(uint32_t j) const {
    assert(j < kSize && "Mat3 column index too large");
    return Vec3(matrix_[0][j], matrix_[1][j], matrix_[2][j]);
}

FloatType Mat3::AlgebraicAddition(uint32_t i, uint32_t j) const {
    assert(i < kSize && j < kSize && "Mat3 element index too large");

    FloatType result = 0.0;
    result += matrix_[(i + 1) % kSize][(j + 1) % kSize] * matrix_[(i + 2) % kSize][(j + 2) % kSize];
    result -= matrix_[(i + 1) % kSize][(j + 2) % kSize] * matrix_[(i + 2) % kSize][(j + 1) % kSize];
    return result;
}

Vec3 Mat3::Apply(Vec2 vector) const {
    return Apply(Vec3(vector, 1.0));
}

Vec3 Mat3::Apply(Vec3 vector) const {
    std::array<FloatType, kSize> transformed;
    for (uint32_t i = 0; i < kSize; ++i) {
        transformed[i] = matrix_[i][0] * vector.X() + matrix_[i][1] * vector.Y() + matrix_[i][2] * vector.Z();
    }
    return Vec3(transformed[0], transformed[1], transformed[2]);
}

Mat3& Mat3::Fill(FloatType valie) {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] = valie;
        }
    }
    return *this;
}

Mat3& Mat3::SetRow(uint32_t i, Vec3 row) {
    assert(i < kSize && "Mat3 row index too large");
    matrix_[i][0] = row.X();
    matrix_[i][1] = row.Y();
    matrix_[i][2] = row.Z();
    return *this;
}

Mat3& Mat3::SetColumn(uint32_t j, Vec3 column) {
    assert(j < kSize && "Mat3 column index too large");
    matrix_[0][j] = column.X();
    matrix_[1][j] = column.Y();
    matrix_[2][j] = column.Z();
    return *this;
}

Mat3& Mat3::Transpose() {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = i + 1; j < kSize; ++j) {
            std::swap(matrix_[i][j], matrix_[j][i]);
        }
    }
    return *this;
}

Mat3 Mat3::Transpose(const Mat3& other) {
    Mat3 result(other);
    result.Transpose();
    return result;
}

Mat3& Mat3::Inverse() {
    *this = Mat3::Inverse(*this);
    return *this;
}

Mat3 Mat3::Inverse(const Mat3& other) {
    Mat3 result = AttachedMatrix(other);

    const FloatType determinant = result.GetRow(0).ScalarProd(other.GetRow(0));
    assert(!Equal(determinant, 0.0) && "Can not inverse singular matrix");

    result.Transpose();
    result /= determinant;
    return result;
}

Mat3 Mat3::AttachedMatrix(const Mat3& other) {
    Mat3 result;
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            result.matrix_[i][j] = other.AlgebraicAddition(i, j);
        }
    }
    return result;
}

Mat3 Mat3::Scale(Vec2 scale) {
    return {{scale.X(), 0.0, 0.0}, {0.0, scale.Y(), 0.0}, {0.0, 0.0, 1.0}};
}

Mat3 Mat3::Scale(FloatType scale_x, FloatType scale_y) {
    return Scale(Vec2(scale_x, scale_y));
}

Mat3 Mat3::Scale(FloatType scale) {
    return Scale(Vec2::Ident(scale));
}

Mat3 Mat3::Translation(Vec2 translation) {
    return {{1.0, 0.0, translation.X()}, {0.0, 1.0, translation.Y()}, {0.0, 0.0, 1.0}};
}

Mat3 Mat3::Translation(FloatType translation_x, FloatType translation_y) {
    return Translation(Vec2(translation_x, translation_y));
}

Mat3 Mat3::Rotation(Vec2 point, FloatType angle) {
    const FloatType c = cos(angle);
    const FloatType s = sin(angle);
    const Mat3 rotation = {{c, -s, 0.0}, {s, c, 0.0}, {0.0, 0.0, 1.0}};

    return Mat3::Translation(point) * rotation * Mat3::Translation(-point);
}

Mat3 Mat3::Basis(Vec2 x, Vec2 y) {
    return {{x.X(), y.X(), 0.0}, {x.Y(), y.Y(), 0.0}, {0.0, 0.0, 1.0}};
}

Mat3 operator*(FloatType scale, const Mat3& matrix) {
    return matrix * scale;
}

std::ostream& operator<<(std::ostream& out, const Mat3& transform) {
    for (uint32_t i = 0; i < Mat3::kSize; ++i) {
        for (uint32_t j = 0; j < Mat3::kSize; ++j) {
            out << transform.GetElement(i, j);

            if (j + 1 < Mat3::kSize) {
                out << ", ";
            }
        }

        if (i + 1 < Mat3::kSize) {
            out << "\n";
        }
    }

    return out;
}

}  // namespace null_engine
