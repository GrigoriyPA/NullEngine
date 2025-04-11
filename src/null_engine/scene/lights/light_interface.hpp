#pragma once

#include <CL/cl_platform.h>
#include <folly/Poly.h>

#include <null_engine/util/geometry/matrix.hpp>

namespace null_engine {

namespace multithread::detail {

struct LightDescription {
    enum LightType {
        LT_NONE,
        LT_AMBIENT,
        LT_DIRECT,
        LT_POINT,
        LT_SPOT,
    };

    cl_int light_type = LT_NONE;
    cl_float3 strength;
    cl_float3 attenuation;
    cl_float3 position;
    cl_float3 direction;
    cl_float2 angle;
};

}  // namespace multithread::detail

struct LightingMaterialSettings {
    Vec3 frag_pos;
    Vec3 view_direction;
    Vec3 normal;
    Vec3 diffuse_color = Vec3(0.0, 0.0, 0.0);
    Vec3 specular_color = Vec3(0.0, 0.0, 0.0);
    FloatType shininess = 0.0;
};

struct ILight {
    using LightDescription = multithread::detail::LightDescription;

    template <class Base>
    struct Interface : Base {
        Vec3 CalculateLighting(const LightingMaterialSettings& settings) const {
            return folly::poly_call<0>(*this, settings);
        }

        LightDescription GetDescription() const {
            return folly::poly_call<1>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::CalculateLighting, &T::GetDescription>;
};

using AnyLight = folly::Poly<ILight>;

struct IMovableLight : folly::PolyExtends<ILight> {
    template <class Base>
    struct Interface : Base {
        void ApplyTransform(const Transform& transform) {
            folly::poly_call<0>(*this, transform);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::ApplyTransform>;
};

using AnyMovableLight = folly::Poly<IMovableLight>;

}  // namespace null_engine
