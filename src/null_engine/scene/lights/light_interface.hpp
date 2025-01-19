#pragma once

#include <folly/Poly.h>

#include <null_engine/util/geometry/matrix4.hpp>

namespace null_engine {

struct LightingMaterialSettings {
    Vec3 frag_pos;
    Vec3 view_direction;
    Vec3 normal;
    Vec3 diffuse_color;
    Vec3 specular_color;
    FloatType shininess = 1.0;
};

struct ILight {
    template <class Base>
    struct Interface : Base {
        Vec3 CalculateLighting(const LightingMaterialSettings& settings) const {
            return folly::poly_call<0>(*this, settings);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::CalculateLighting>;
};

using AnyLight = folly::Poly<ILight>;

struct IMovableLight : folly::PolyExtends<ILight> {
    template <class Base>
    struct Interface : Base {
        void ApplyTransform(const Mat4& transform) {
            folly::poly_call<0>(*this, transform);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::ApplyTransform>;
};

using AnyMovableLight = folly::Poly<IMovableLight>;

}  // namespace null_engine
