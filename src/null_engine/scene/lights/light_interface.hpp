#pragma once

#include <folly/Poly.h>

#include <null_engine/util/geometry/vector3.hpp>

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

}  // namespace null_engine
