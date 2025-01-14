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

FloatType Mat4::GetElement(uint32_t i, uint32_t j) const {
    assert(i < kSize && j < kSize && "Mat4 element index too large");

    return matrix_[i][j];
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

Vec3 Mat4::Apply(Vec3 vector) const {
    FloatType transformed[kSize];
    for (uint32_t i = 0; i < kSize; ++i) {
        transformed[i] = matrix_[i][0] * vector.GetX() + matrix_[i][1] * vector.GetY() + matrix_[i][2] * vector.GetZ() +
                         matrix_[i][3] * vector.GetH();
    }
    return Vec3(transformed[0], transformed[1], transformed[2], transformed[3]);
}

void Mat4::Fill(FloatType valie) {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] = valie;
        }
    }
}

Mat4 Mat4::Scale(Vec3 scale) {
    return Mat4(
        {{scale.GetX(), 0.0, 0.0, 0.0},
         {0.0, scale.GetY(), 0.0, 0.0},
         {0.0, 0.0, scale.GetZ(), 0.0},
         {0.0, 0.0, 0.0, 1.0}}
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
        {{1.0, 0.0, 0.0, translation.GetX()},
         {0.0, 1.0, 0.0, translation.GetY()},
         {0.0, 0.0, 1.0, translation.GetZ()},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Mat4 Mat4::Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z) {
    return Translation(Vec3(translation_x, translation_y, translation_z));
}

Mat4 Mat4::Rotation(Vec3 axis, FloatType angle) {
    axis.Normalize();

    const FloatType x = axis.GetX();
    const FloatType y = axis.GetY();
    const FloatType z = axis.GetZ();
    const FloatType c = cos(angle);
    const FloatType s = sin(angle);

    return Mat4(
        {{c + x * x * (1.0 - c), x * y * (1.0 - c) - z * s, x * z * (1.0 - c) + y * s, 0.0},
         {y * x * (1.0 - c) + z * s, c + y * y * (1.0 - c), y * z * (1.0 - c) - x * s, 0.0},
         {z * x * (1.0 - c) - y * s, z * y * (1.0 - c) + x * s, c + z * z * (1.0 - c), 0.0},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Mat4 Mat4::Basis(Vec3 x, Vec3 y, Vec3 z) {
    return Mat4(
        {{x.GetX(), y.GetX(), z.GetX(), 0.0},
         {x.GetY(), y.GetY(), z.GetY(), 0.0},
         {x.GetZ(), y.GetZ(), z.GetZ(), 0.0},
         {0.0, 0.0, 0.0, 1.0}}
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
