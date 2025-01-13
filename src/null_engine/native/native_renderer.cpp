#include "native_renderer.hpp"

#include <cstdint>
#include <null_engine/native/rasterization/native_rasterizer_context.hpp>

namespace null_engine::native {

Renderer::Renderer(const RendererSettings& settings)
    : rasterizer_(RasterizerContext(settings.view_width, settings.view_height)) {
}

void Renderer::SetCamera(folly::Poly<const generic::ICamera&> camera) {
    ndc_transform_ = camera->GetNdcTransform();
    rasterizer_.GetContext().Rewind();
}

void Renderer::RenderObject(folly::Poly<const generic::IMeshObject&> object) {
    const auto& vertices = object->GetVertices();
    for (uint64_t index : object->GetIndices()) {
        auto vertex = vertices[index];
        vertex.ApplyNdcTransform(ndc_transform_);

        rasterizer_.DrawPoint(vertex);
    }
}

void Renderer::SaveRenderingResults(folly::Poly<generic::IRenderingConsumer&> consumer) const {
    consumer->OnRenderedTexture(rasterizer_.GetContext().colors_buffer);
}

}  // namespace null_engine::native
