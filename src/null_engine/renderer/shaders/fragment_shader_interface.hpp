#pragma once

#include <folly/Poly.h>

#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/util/geometry/vector.hpp>

#include "vertex_shader.hpp"

namespace null_engine::native::detail {

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

}  // namespace null_engine::native::detail
