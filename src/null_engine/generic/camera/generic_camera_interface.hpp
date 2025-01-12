#pragma once

#include <folly/Poly.h>

#include <null_engine/util/geometry/transformation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::generic {

struct ICamera {
    template <class Base>
    struct Interface : Base {
        // Transform from scene space to normalized device coordinates:
        // -- NDC coordinates is box [-1, 1] for all coordinates
        util::Transform GetNdcTransform() const {
            return folly::poly_call<0>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetNdcTransform>;
};

struct IMovableCamera : folly::PolyExtends<ICamera> {
    template <class Base>
    struct Interface : Base {
        // Camera position in scene space
        util::Vec3 GetPosition() const {
            return folly::poly_call<0>(*this);
        }

        // Camera right-hand normalized orientation in scene space
        util::Vec3 GetDirection() const {
            return folly::poly_call<1>(*this);
        }

        util::Vec3 GetHorizon() const {
            return folly::poly_call<2>(*this);
        }

        util::Vec3 GetVertical() const {
            return folly::poly_call<3>(*this);
        }

        void Move(util::Vec3 translation) {
            folly::poly_call<4>(*this, translation);
        }

        void Rotate(util::Vec3 axis, util::FloatType angle) {
            folly::poly_call<5>(*this, axis, angle);
        }
    };

    template <class T>
    using Members =
        folly::PolyMembers<&T::GetPosition, &T::GetDirection, &T::GetHorizon, &T::GetVertical, &T::Move, &T::Rotate>;
};

}  // namespace null_engine::generic
