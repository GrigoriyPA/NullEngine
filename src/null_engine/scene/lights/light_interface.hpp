#pragma once

#include <CL/cl_platform.h>
#include <folly/Poly.h>

#include <null_engine/util/geometry/matrix.hpp>

#include "depth_buffer.hpp"

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
    cl_float4 shadow_space[4];
    cl_int2 shadow_size;
};

}  // namespace multithread::detail

struct LightingMaterialSettings {
    Vec3 frag_pos;
    Vec3 view_direction;
    Vec3 normal;
    Vec3 diffuse_color = Vec3(0.0, 0.0, 0.0);
    Vec3 specular_color = Vec3(0.0, 0.0, 0.0);
    float shininess = 0.0;
    bool shadow = true;
};

struct ILight {
    using LightDescription = multithread::detail::LightDescription;
    using ShadowInfo = detail::ShadowInfo;
    using DepthBuffer = detail::DepthBuffer;

    template <class Base>
    struct Interface : Base {
        Vec3 CalculateLighting(const LightingMaterialSettings& settings, DepthBuffer depth) const {
            return folly::poly_call<0>(*this, settings, depth);
        }

        LightDescription GetDescription() const {
            return folly::poly_call<1>(*this);
        }

        std::optional<ShadowInfo> GetShadowInfo() const {
            return folly::poly_call<2>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::CalculateLighting, &T::GetDescription, &T::GetShadowInfo>;
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
