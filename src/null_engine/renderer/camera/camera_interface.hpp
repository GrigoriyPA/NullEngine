#pragma once

#include <folly/Poly.h>

#include <null_engine/util/geometry/matrix4.hpp>

namespace null_engine {

struct ICamera {
    template <class Base>
    struct Interface : Base {
        Mat4 GetNdcTransform() const {
            return folly::poly_call<0>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetNdcTransform>;
};

using AnyCamera = folly::Poly<ICamera>;
using AnyCameraRef = folly::Poly<ICamera&>;

struct IMovableCamera : folly::PolyExtends<ICamera> {
    template <class Base>
    struct Interface : Base {
        void Move(FloatType direct_move, FloatType horizon_move, FloatType vertical_move) {
            folly::poly_call<0>(*this, direct_move, horizon_move, vertical_move);
        }

        void Rotate(FloatType yaw_rotation, FloatType pitch_rotation, FloatType roll_rotation) {
            folly::poly_call<1>(*this, yaw_rotation, pitch_rotation, roll_rotation);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::Move, &T::Rotate>;
};

using AnyMovableCamera = folly::Poly<IMovableCamera>;
using AnyMovableCameraRef = folly::Poly<IMovableCamera&>;

}  // namespace null_engine
