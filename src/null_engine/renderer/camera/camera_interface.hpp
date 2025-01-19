#pragma once

#include <folly/Poly.h>

#include <null_engine/util/geometry/matrix4.hpp>

namespace null_engine {

struct ICamera {
    template <class Base>
    struct Interface : Base {
        Vec3 GetViewPos() const {
            return folly::poly_call<0>(*this);
        }

        Mat4 GetNdcTransform() const {
            return folly::poly_call<1>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetViewPos, &T::GetNdcTransform>;
};

using AnyCameraRef = folly::Poly<const ICamera&>;

}  // namespace null_engine
