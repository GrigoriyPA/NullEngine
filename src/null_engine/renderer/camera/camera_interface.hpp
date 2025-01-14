#pragma once

#include <folly/Poly.h>

#include <null_engine/util/geometry/matrix4.hpp>

namespace null_engine {

struct ICamera {
    template <class Base>
    struct Interface : Base {
        Mat4 GetNdcMat4() const {
            return folly::poly_call<0>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetNdcMat4>;
};

using AnyCamera = folly::Poly<ICamera>;
using AnyCameraRef = folly::Poly<ICamera&>;

struct IMovableCamera : folly::PolyExtends<ICamera> {
    template <class Base>
    struct Interface : Base {
        Vec3 GetPosition() const {
            return folly::poly_call<0>(*this);
        }

        Vec3 GetDirection() const {
            return folly::poly_call<1>(*this);
        }

        Vec3 GetHorizon() const {
            return folly::poly_call<2>(*this);
        }

        Vec3 GetVertical() const {
            return folly::poly_call<3>(*this);
        }

        void Move(Vec3 translation) {
            folly::poly_call<4>(*this, translation);
        }

        void Rotate(Vec3 axis, FloatType angle) {
            folly::poly_call<5>(*this, axis, angle);
        }
    };

    template <class T>
    using Members =
        folly::PolyMembers<&T::GetPosition, &T::GetDirection, &T::GetHorizon, &T::GetVertical, &T::Move, &T::Rotate>;
};

using AnyMovableCamera = folly::Poly<IMovableCamera>;
using AnyMovableCameraRef = folly::Poly<IMovableCamera&>;

}  // namespace null_engine
