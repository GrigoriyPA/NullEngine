#pragma once

#include <cstdint>
#include <initializer_list>

#include "constants.hpp"
#include "vector_3d.hpp"

namespace null_engine::util {

class Transform {
public:
    static constexpr uint32_t kSize = 4;

public:
    Transform();
    Transform(std::initializer_list<std::initializer_list<FloatType>> init);

    FloatType GetElement(uint32_t i, uint32_t j) const;

    Transform& Transpose();

    [[nodiscard]] Transform Transposed() const;

    Transform& ComposeBefore(const Transform& other);

    [[nodiscard]] Transform ComposedBefore(const Transform& other) const;

    Transform& ComposeAfter(const Transform& other);

    [[nodiscard]] Transform ComposedAfter(const Transform& other) const;

    Vec3 Apply(Vec3 vector) const;

public:
    static Transform Scale(Vec3 scale);

    static Transform Scale(FloatType scale_x, FloatType scale_y, FloatType scale_z);

    static Transform Scale(FloatType scale);

    static Transform Translation(Vec3 translation);

    static Transform Translation(FloatType translation_x, FloatType translation_y, FloatType translation_z);

    static Transform Rotation(Vec3 axis, FloatType angle);

    // Transformation from specified basis
    static Transform Basis(Vec3 x, Vec3 y, Vec3 z);

    // Camera projection transforms to NDC space [-1, 1]^3
    static Transform BoxProjection(FloatType width, FloatType height, FloatType depth);

private:
    void Fill(FloatType valie);

private:
    FloatType matrix_[kSize][kSize];
};

std::ostream& operator<<(std::ostream& out, const Transform& transform);

}  // namespace null_engine::util
