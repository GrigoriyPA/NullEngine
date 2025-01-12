#pragma once

#include <null_engine/generic/renderer/generic_renderer.hpp>
#include <null_engine/native/rasterization/native_rasterizer.hpp>
#include <null_engine/native/rasterization/native_rasterizer_context.hpp>
#include <null_engine/util/generic/templates.hpp>
#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::native {

//
// Rasterizer and renderer settings
// -> view_width -- width of output texture in pixels
// -> view_height -- height of output texture in pixels
//
struct RendererSettings {
    uint64_t view_width;
    uint64_t view_height;
};

//
// Renderer for 3D object on CPU
//
class Renderer : public generic::Renderer, public util::SharedConstructable<Renderer> {
public:
    using Ptr = std::shared_ptr<Renderer>;

public:
    explicit Renderer(const RendererSettings& settings);

public:
    void SetCamera(const generic::Camera& camera) override;

    void RenderObject(const generic::MeshObject& object) override;

    void SaveRenderingResults(generic::RenderingConsumer& consumer) const override;

private:
    util::Transform ndc_transform_;
    RasterizerContext rasterizer_context_;

    Rasterizer rasterizer_;
};

}  // namespace null_engine::native
