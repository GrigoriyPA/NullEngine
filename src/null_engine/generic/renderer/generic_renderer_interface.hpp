#pragma once

#include <folly/Poly.h>

#include <null_engine/generic/camera/generic_camera_interface.hpp>
#include <null_engine/generic/mesh/generic_mesh_interface.hpp>

#include "generic_consumer_interface.hpp"

namespace null_engine::generic {

struct IRenderer {
    template <class Base>
    struct Interface : Base {
        void SetCamera(folly::Poly<const ICamera&> camera) {
            folly::poly_call<0>(*this, std::move(camera));
        }

        void RenderObject(folly::Poly<const IMeshObject&> object) {
            folly::poly_call<1>(*this, std::move(object));
        }

        void SaveRenderingResults(folly::Poly<IRenderingConsumer&> consumer) const {
            folly::poly_call<2>(*this, std::move(consumer));
        }
    };

    template <class T>
    using Members = folly::PolyMembers<&T::SetCamera, &T::RenderObject, &T::SaveRenderingResults>;
};

}  // namespace null_engine::generic
