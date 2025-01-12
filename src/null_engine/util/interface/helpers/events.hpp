#pragma once

#include <folly/Poly.h>

#include <SFML/Window/Event.hpp>
#include <null_engine/util/generic/provider.hpp>

namespace null_engine::util {

struct IEventsClient {
    template <class Base>
    struct Interface : Base {
        void OnEvent(const sf::Event& event) {
            folly::poly_call<0>(*this, event);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::OnEvent>;
};

struct IEventsProvider : folly::PolyExtends<IProvider<IEventsClient>> {
    template <class Base>
    struct Interface : Base {
        void DispatchEvent(const sf::Event& event) {
            folly::poly_call<0>(*this, event);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::DispatchEvent>;
};

using EventsProviderRef = folly::Poly<IProvider<IEventsClient>&>;

folly::Poly<IEventsProvider> CreateEventsProvider();

}  // namespace null_engine::util
