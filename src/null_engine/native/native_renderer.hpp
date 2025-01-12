#pragma once

#include <folly/Poly.h>

#include <null_engine/generic/camera/generic_camera_interface.hpp>
#include <null_engine/generic/mesh/generic_mesh_interface.hpp>
#include <null_engine/generic/renderer/generic_consumer_interface.hpp>
#include <null_engine/native/rasterization/native_rasterizer.hpp>
#include <null_engine/util/geometry/transformation.hpp>

namespace null_engine::native {

struct RendererSettings {
    uint64_t view_width;
    uint64_t view_height;
};

class Renderer {
public:
    explicit Renderer(const RendererSettings& settings);

    void SetCamera(folly::Poly<const generic::ICamera&> camera);

    void RenderObject(folly::Poly<const generic::IMeshObject&> object);

    void SaveRenderingResults(folly::Poly<generic::IRenderingConsumer&> consumer) const;

private:
    util::Transform ndc_transform_;
    Rasterizer rasterizer_;
};

}  // namespace null_engine::native
