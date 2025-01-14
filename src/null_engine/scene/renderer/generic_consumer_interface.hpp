#pragma once

#include <folly/Poly.h>

#include <cstdint>
#include <vector>

namespace null_engine {

struct IRenderingConsumer {
    template <class Base>
    struct Interface : Base {
        // texture -- array of pixel colors with size 4 * view_width * view_height
        // (4 color coordinates - R, G, B, A)
        void OnRenderedTexture(const std::vector<uint8_t>& texture) {
            folly::poly_call<0>(*this, texture);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::OnRenderedTexture>;
};

}  // namespace null_engine
