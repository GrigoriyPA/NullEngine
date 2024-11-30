#include <null_engine/native/native_renderer.hpp>

#include <cstdint>

namespace null_engine::native {

//// Renderer

Renderer::Renderer(const RendererSettings& settings)
    : rasterizer_context_(settings.view_width, settings.view_height)
    , rasterizer_(rasterizer_context_) {
}

void Renderer::SetCamera(const generic::Camera& camera) {
    rasterizer_context_.Rewind();
}

void Renderer::RenderObject(const generic::MeshObject& object) {
    const auto& vertices = object.GetVertices();
    for (uint64_t index : object.GetIndices()) {
        rasterizer_.DrawPoint(vertices[index]);
    }
}

void Renderer::SaveRenderingResults(generic::RenderingConsumer& consumer) const {
    consumer.OnRenderedTexture(rasterizer_context_.colors_buffer);
}

}  // namespace null_engine::native
