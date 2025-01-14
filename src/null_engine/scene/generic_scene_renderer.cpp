#include "generic_scene_renderer.hpp"

namespace null_engine {

SceneRenderer::SceneRenderer(folly::Poly<IRenderer> renderer, folly::Poly<IRenderingConsumer&> rendering_consumer)
    : renderer_(std::move(renderer))
    , rendering_consumer_(std::move(rendering_consumer)) {
}

void SceneRenderer::Render(const Scene& scene, folly::Poly<const ICamera&> camera) {
    renderer_.SetCamera(camera);

    for (const auto& object : scene.GetObjects()) {
        renderer_.RenderObject(object);
    }

    renderer_.SaveRenderingResults(rendering_consumer_);
}

}  // namespace null_engine
