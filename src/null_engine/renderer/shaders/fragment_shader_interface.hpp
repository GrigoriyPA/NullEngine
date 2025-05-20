#pragma once

#include <folly/Poly.h>

#include <null_engine/acceleration/kernel.hpp>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/scene/lights/light_interface.hpp>
#include <null_engine/util/geometry/vector.hpp>

#include "vertex_shader.hpp"

namespace null_engine {

namespace native::detail {

struct IFragmentShader {
    using InterpolationParams = null_engine::detail::InterpolationParams;

    template <class Base>
    struct Interface : Base {
        Vec4 GetPointColor(const InterpolationParams& params, bool& discard) const {
            return folly::poly_call<0>(*this, params, discard);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetPointColor>;
};

using AnyFragmentShader = folly::Poly<IFragmentShader>;

using AnyFragmentShaderRef = folly::Poly<const IFragmentShader&>;

}  // namespace native::detail

namespace multithread::detail {

struct IFragmentShader {
    using InterpolationParams = null_engine::detail::InterpolationParams;

    template <class Base>
    struct Interface : Base {
        Program GetProgram() const {
            return folly::poly_call<0>(*this);
        }

        ArgsInfo GetArgs() const {
            return folly::poly_call<1>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetProgram, &T::GetArgs>;
};

using AnyFragmentShader = folly::Poly<IFragmentShader>;

using AnyFragmentShaderRef = folly::Poly<const IFragmentShader&>;

}  // namespace multithread::detail

}  // namespace null_engine
