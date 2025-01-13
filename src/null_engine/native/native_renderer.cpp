#include "native_renderer.hpp"

#include <cstdint>
#include <null_engine/generic/mesh/generic_vertex.hpp>
#include <null_engine/native/rasterization/native_rasterizer_context.hpp>

namespace null_engine::native {

namespace {

void ApplyNdcTransform(generic::Vertex& vertex, const util::Transform& ndc_transform) {
    auto position = ndc_transform.Apply(vertex.GetPosition());
    position /= position.GetH();
    position.H() = 1.0 / position.GetH();

    vertex.SetPosition(position);
}

}  // anonymous namespace

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
        ApplyNdcTransform(vertex, ndc_transform_);

        rasterizer_.DrawPoint(vertex);
    }
}

void Renderer::SaveRenderingResults(folly::Poly<generic::IRenderingConsumer&> consumer) const {
    consumer->OnRenderedTexture(rasterizer_.GetContext().colors_buffer);
}

}  // namespace null_engine::native
