#include "transformation.hpp"

#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <sstream>
#include <utility>

namespace null_engine::util {

Transform::Transform() {
    Fill(0.0);

    for (uint32_t i = 0; i < kSize; ++i) {
        matrix_[i][i] = 1.0;
    }
}

Transform::Transform(std::initializer_list<std::initializer_list<FloatType>> init) {
    Ensure(
        init.size() == kSize, std::stringstream() << "Unexpected initializer list size " << init.size()
                                                  << " for transform it should be equal " << kSize
    );

    for (uint32_t i = 0; const auto& init_row : init) {
        Ensure(
            init_row.size() == kSize, std::stringstream()
                                          << "Unexpected initializer list row " << i << " size " << init.size()
                                          << " for transform it should be equal " << kSize
        );

        for (uint32_t j = 0; FloatType init_element : init_row) {
            matrix_[i][j++] = init_element;
        }
        ++i;
    }
}

FloatType Transform::GetElement(uint32_t i, uint32_t j) const {
    Ensure(
        i < kSize && j < kSize,
        std::stringstream() << "Invalid index (" << i << ", " << j << ") for transform it should be at most " << kSize
    );

    return matrix_[i][j];
}

Transform& Transform::Transpose() {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = i + 1; j < kSize; ++j) {
            std::swap(matrix_[i][j], matrix_[j][i]);
        }
    }
    return *this;
}

Transform Transform::Transposed() const {
    Transform copy(*this);
    return copy.Transpose();
}

Transform& Transform::ComposeBefore(const Transform& other) {
    Transform current(*this);
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] = 0.0;

            for (uint32_t k = 0; k < kSize; ++k) {
                matrix_[i][j] += current.matrix_[i][k] * other.matrix_[k][j];
            }
        }
    }
    return *this;
}

Transform Transform::ComposedBefore(const Transform& other) const {
    Transform copy(*this);
    return copy.ComposeBefore(other);
}

Transform& Transform::ComposeAfter(const Transform& other) {
    Transform current(*this);
    *this = other;
    ComposeBefore(current);

    return *this;
}

Transform Transform::ComposedAfter(const Transform& other) const {
    Transform copy(*this);
    return copy.ComposeAfter(other);
}

Vec3 Transform::Apply(Vec3 vector) const {
    FloatType transformed[kSize];
    for (uint32_t i = 0; i < kSize; ++i) {
        transformed[i] = matrix_[i][0] * vector.GetX() + matrix_[i][1] * vector.GetY() + matrix_[i][2] * vector.GetZ() +
                         matrix_[i][3] * vector.GetH();
    }
    return Vec3(transformed[0], transformed[1], transformed[2], transformed[3]);
}

void Transform::Fill(FloatType valie) {
    for (uint32_t i = 0; i < kSize; ++i) {
        for (uint32_t j = 0; j < kSize; ++j) {
            matrix_[i][j] = valie;
        }
    }
}

Transform Transform::Scale(Vec3 scale) {
    return Transform(
        {{scale.GetX(), 0.0, 0.0, 0.0},
         {0.0, scale.GetY(), 0.0, 0.0},
         {0.0, 0.0, scale.GetZ(), 0.0},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Transform Transform::Scale(FloatType scale_x, FloatType scale_y, FloatType scale_z) {
    return Scale(Vec3(scale_x, scale_y, scale_z));
}

Transform Transform::Scale(FloatType scale) {
    return Scale(Vec3(scale));
}

Transform Transform::Translation(Vec3 translation) {
    return Transform(
        {{1.0, 0.0, 0.0, translation.GetX()},
         {0.0, 1.0, 0.0, translation.GetY()},
         {0.0, 0.0, 1.0, translation.GetZ()},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Transform Transform::Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z) {
    return Translation(Vec3(translation_x, translation_y, translation_z));
}

Transform Transform::Rotation(Vec3 axis, FloatType angle) {
    axis.Normalize();

    const FloatType x = axis.GetX();
    const FloatType y = axis.GetY();
    const FloatType z = axis.GetZ();
    const FloatType c = cos(angle);
    const FloatType s = sin(angle);

    return Transform(
        {{c + x * x * (1.0 - c), x * y * (1.0 - c) - z * s, x * z * (1.0 - c) + y * s, 0.0},
         {y * x * (1.0 - c) + z * s, c + y * y * (1.0 - c), y * z * (1.0 - c) - x * s, 0.0},
         {z * x * (1.0 - c) - y * s, z * y * (1.0 - c) + x * s, c + z * z * (1.0 - c), 0.0},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Transform Transform::Basis(Vec3 x, Vec3 y, Vec3 z) {
    return Transform(
        {{x.GetX(), y.GetX(), z.GetX(), 0.0},
         {x.GetY(), y.GetY(), z.GetY(), 0.0},
         {x.GetZ(), y.GetZ(), z.GetZ(), 0.0},
         {0.0, 0.0, 0.0, 1.0}}
    );
}

Transform Transform::BoxProjection(FloatType width, FloatType height, FloatType depth) {
    return Transform::Scale(2.0 / width, 2.0 / height, 2.0 / depth)
        .ComposeAfter(Transform::Translation(Vec3(0.0, 0.0, -1.0)));
}

std::ostream& operator<<(std::ostream& out, const Transform& transform) {
    for (uint32_t i = 0; i < Transform::kSize; ++i) {
        for (uint32_t j = 0; j < Transform::kSize; ++j) {
            out << transform.GetElement(i, j);

            if (j + 1 < Transform::kSize) {
                out << ", ";
            }
        }

        if (i + 1 < Transform::kSize) {
            out << "\n";
        }
    }

    return out;
}

}  // namespace null_engine::util
