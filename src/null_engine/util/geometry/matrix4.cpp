#include "matrix4.hpp"

#include <cassert>

#include "helpers.hpp"

namespace null_engine {

Mat4::Mat4() {
    Fill(0.0);

    for (uint32_t i = 0; i < kSize; ++i) {
        matrix_[i][i] = 1.0;
    }
}

Mat4::Mat4(std::initializer_list<std::initializer_list<FloatType>> init) {
    assert(init.size() == kSize && "Invalid initializer list size for transform");

    for (uint32_t i = 0; const auto& init_row : init) {
        assert(init_row.size() == kSize && "Invalid initializer list row size for transform");

        for (uint32_t j = 0; FloatType init_element : init_row) {
            matrix_[i][j++] = init_element;
        }
        ++i;
    }
}

Mat4::Mat4(std::initializer_list<Vec4> rows) {
    assert(rows.size() == kSize && "Invalid initializer list size for transform");

    for (uint32_t i = 0; const auto& init_row : rows) {
        SetRow(i++, init_row);
    }
}

Mat4& Mat4::operator*=(const Mat4& other) {
    return *this = *this * other;
}

Mat4 operator*(const Mat4& left, const Mat4& right) {
    Mat4 result;
    for (uint32_t i = 0; i < Mat4::kSize; ++i) {
        for (uint32_t j = 0; j < Mat4::kSize; ++j) {
            result.matrix_[i][j] = 0.0;

            for (uint32_t k = 0; k < Mat4::kSize; ++k) {
                result.matrix_[i][j] += left.matrix_[i][k] * right.matrix_[k][j];
            }
        }
    }
    return result;
}

Mat4& Mat4::operator*=(FloatType scale) {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] *= scale;
        }
    }
    return *this;
}

Mat4 operator*(Mat4 matrix, FloatType scale) {
    matrix *= scale;
    return matrix;
}

Mat4& Mat4::operator/=(FloatType scale) {
    assert(!Equal(scale, 0.0) && "Division by zero");

    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] /= scale;
        }
    }
    return *this;
}

Mat4 operator/(Mat4 matrix, FloatType scale) {
    matrix /= scale;
    return matrix;
}

FloatType Mat4::GetElement(uint32_t i, uint32_t j) const {
    assert(i < kSize && j < kSize && "Mat4 element index too large");

    return matrix_[i][j];
}

Vec4 Mat4::GetRow(uint32_t i) const {
    assert(i < kSize && "Mat4 row index too large");
    return Vec4(matrix_[i][0], matrix_[i][1], matrix_[i][2], matrix_[i][3]);
}

Vec4 Mat4::GetColumn(uint32_t j) const {
    assert(j < kSize && "Mat4 column index too large");
    return Vec4(matrix_[0][j], matrix_[1][j], matrix_[2][j], matrix_[3][j]);
}

Vec4 Mat4::Apply(Vec3 vector) const {
    return Apply(Vec4(vector, 1.0));
}

Vec4 Mat4::Apply(Vec4 vector) const {
    std::array<FloatType, kSize> transformed;
    for (uint32_t i = 0; i < kSize; ++i) {
        transformed[i] = matrix_[i][0] * vector.X() + matrix_[i][1] * vector.Y() + matrix_[i][2] * vector.Z() +
                         matrix_[i][3] * vector.H();
    }
    return Vec4(transformed[0], transformed[1], transformed[2], transformed[3]);
}

Mat4& Mat4::Fill(FloatType valie) {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] = valie;
        }
    }
    return *this;
}

Mat4& Mat4::SetRow(uint32_t i, Vec4 row) {
    assert(i < kSize && "Mat4 row index too large");
    matrix_[i][0] = row.X();
    matrix_[i][1] = row.Y();
    matrix_[i][2] = row.Z();
    matrix_[i][3] = row.H();
    return *this;
}

Mat4& Mat4::SetColumn(uint32_t j, Vec4 column) {
    assert(j < kSize && "Mat4 column index too large");
    matrix_[0][j] = column.X();
    matrix_[1][j] = column.Y();
    matrix_[2][j] = column.Z();
    matrix_[3][j] = column.H();
    return *this;
}

Mat4& Mat4::Transpose() {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = i + 1; j < kSize; ++j) {
            std::swap(matrix_[i][j], matrix_[j][i]);
        }
    }
    return *this;
}

Mat4 Mat4::Transpose(const Mat4& other) {
    Mat4 result(other);
    result.Transpose();
    return result;
}

Mat3 Mat4::ToMat3(const Mat4& tranform) {
    return {tranform.GetRow(0).XYZ(), tranform.GetRow(1).XYZ(), tranform.GetRow(2).XYZ()};
}

Mat4 Mat4::FromMat3(const Mat3& tranform) {
    return {
        Vec4(tranform.GetRow(0), 0.0), Vec4(tranform.GetRow(1), 0.0), Vec4(tranform.GetRow(2), 0.0),
        Vec4(0.0, 0.0, 0.0, 1.0)
    };
}

Mat3 Mat4::NormalTransform(const Mat4& tranform) {
    return Mat3::Inverse(Mat4::ToMat3(tranform)).Transpose();
}

Mat4 Mat4::Scale(Vec3 scale) {
    return Mat4(
        {{scale.X(), 0.0, 0.0, 0.0}, {0.0, scale.Y(), 0.0, 0.0}, {0.0, 0.0, scale.Z(), 0.0}, {0.0, 0.0, 0.0, 1.0}}
    );
}

Mat4 Mat4::Scale(FloatType scale_x, FloatType scale_y, FloatType scale_z) {
    return Scale(Vec3(scale_x, scale_y, scale_z));
}

Mat4 Mat4::Scale(FloatType scale) {
    return Scale(Vec3::Ident(scale));
}

Mat4 Mat4::Translation(Vec3 translation) {
    return Mat4(
        {{1.0, 0.0, 0.0, translation.X()},
         {0.0, 1.0, 0.0, translation.Y()},
         {0.0, 0.0, 1.0, translation.Z()},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Mat4 Mat4::Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z) {
    return Translation(Vec3(translation_x, translation_y, translation_z));
}

Mat4 Mat4::Rotation(Vec3 axis, FloatType angle) {
    axis.Normalize();

    const FloatType x = axis.X();
    const FloatType y = axis.Y();
    const FloatType z = axis.Z();
    const FloatType c = cos(angle);
    const FloatType ic = 1.0 - c;
    const FloatType s = sin(angle);

    return Mat4(
        {{c + x * x * ic, x * y * ic - z * s, x * z * ic + y * s, 0.0},
         {y * x * ic + z * s, c + y * y * ic, y * z * ic - x * s, 0.0},
         {z * x * ic - y * s, z * y * ic + x * s, c + z * z * ic, 0.0},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Mat4 Mat4::Basis(Vec3 x, Vec3 y, Vec3 z) {
    return Mat4(
        {{x.X(), y.X(), z.X(), 0.0}, {x.Y(), y.Y(), z.Y(), 0.0}, {x.Z(), y.Z(), z.Z(), 0.0}, {0.0, 0.0, 0.0, 1.0}}
    );
}

Mat4 Mat4::BoxProjection(FloatType width, FloatType height, FloatType depth) {
    return Mat4::Translation(Vec3(0.0, 0.0, -1.0)) * Mat4::Scale(2.0 / width, 2.0 / height, 2.0 / depth);
}

Mat4 Mat4::PerspectiveProjection(FloatType fov, FloatType ratio, FloatType min_distance, FloatType max_distance) {
    assert(Less(0.0, min_distance) && "Min projection distance should be positive");
    assert(Less(min_distance, max_distance) && "Min and max projection distances invalid");

    const FloatType t = tan(fov / 2.0);
    assert(Less(0.0, t) && "Invalid fov, value should be in interval (0; PI)");

    auto transform = Mat4::Scale(1.0 / t, ratio / t, (max_distance + min_distance) / (max_distance - min_distance)) *
                     Mat4::Translation(0.0, 0.0, -2.0 * max_distance * min_distance / (max_distance + min_distance));

    transform.matrix_[kSize - 1][kSize - 1] = 0.0;
    transform.matrix_[kSize - 1][kSize - 2] = 1.0;

    return transform;
}

Mat4 operator*(FloatType scale, const Mat4& matrix) {
    return matrix * scale;
}

std::ostream& operator<<(std::ostream& out, const Mat4& transform) {
    for (uint32_t i = 0; i < Mat4::kSize; ++i) {
        for (uint32_t j = 0; j < Mat4::kSize; ++j) {
            out << transform.GetElement(i, j);

            if (j + 1 < Mat4::kSize) {
                out << ", ";
            }
        }

        if (i + 1 < Mat4::kSize) {
            out << "\n";
        }
    }

    return out;
}

}  // namespace null_engine
