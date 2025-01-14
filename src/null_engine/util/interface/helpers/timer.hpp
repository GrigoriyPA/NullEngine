#pragma once

#include <folly/Poly.h>

#include <null_engine/util/generic/provider.hpp>
#include <null_engine/util/geometry/constants.hpp>

namespace null_engine {

struct ITimerClient {
    template <class Base>
    struct Interface : Base {
        void Update(FloatType delta_time) {
            folly::poly_call<0>(*this, delta_time);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::Update>;
};

struct ITimerProvider : folly::PolyExtends<IProvider<ITimerClient>> {
    template <class Base>
    struct Interface : Base {
        void RefreshClients() {
            folly::poly_call<0>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::RefreshClients>;
};

template <typename Derived>
using TimerClientBase = ClientBase<Derived, ITimerClient>;

using TimerProviderRef = folly::Poly<IProvider<ITimerClient>&>;

folly::Poly<ITimerProvider> CreateTimerProvider();

}  // namespace null_engine
