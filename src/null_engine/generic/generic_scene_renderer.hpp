#pragma once

#include <folly/Poly.h>

#include <null_engine/generic/camera/generic_camera_interface.hpp>
#include <null_engine/generic/renderer/generic_consumer_interface.hpp>
#include <null_engine/generic/renderer/generic_renderer_interface.hpp>

#include "generic_scene.hpp"

namespace null_engine::generic {

class SceneRenderer {
public:
    SceneRenderer(folly::Poly<IRenderer> renderer, folly::Poly<IRenderingConsumer&> rendering_consumer);

    void Render(const Scene& scene, folly::Poly<const ICamera&> camera);

private:
    folly::Poly<IRenderer> renderer_;
    const folly::Poly<IRenderingConsumer&> rendering_consumer_;
};

}  // namespace null_engine::generic
