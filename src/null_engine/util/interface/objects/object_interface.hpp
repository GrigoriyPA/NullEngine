#pragma once

#include <folly/Poly.h>

#include <SFML/Graphics/Drawable.hpp>

namespace null_engine::util {

struct IInterfaceObject {
    template <class Base>
    struct Interface : Base {
        void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            folly::poly_call<0>(*this, target, states);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::draw>;
};

}  // namespace null_engine::util
