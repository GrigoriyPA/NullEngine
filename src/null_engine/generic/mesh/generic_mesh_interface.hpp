#pragma once

#include <folly/Poly.h>

#include <cstdint>
#include <vector>

#include "generic_vertex.hpp"

namespace null_engine::generic {

struct IMeshObject {
    template <class Base>
    struct Interface : Base {
        const std::vector<Vertex>& GetVertices() const {
            return folly::poly_call<0>(*this);
        }

        const std::vector<uint64_t>& GetIndices() const {
            return folly::poly_call<1>(*this);
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::GetVertices, &T::GetIndices>;
};

}  // namespace null_engine::generic
