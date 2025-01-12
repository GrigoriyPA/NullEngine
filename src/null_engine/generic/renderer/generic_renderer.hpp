#pragma once

#include <memory>
#include <null_engine/generic/camera/generic_camera.hpp>
#include <null_engine/generic/mesh/generic_mesh.hpp>

#include "generic_rendering_consumer.hpp"

namespace null_engine::generic {

//
// Common interface for all rendering backeds
//
class Renderer {
public:
    using Ptr = std::shared_ptr<Renderer>;

public:
    // Update rendering camera and prepare rendering context
    virtual void SetCamera(const Camera& camera) = 0;

    // Perform object rendering into current context
    virtual void RenderObject(const MeshObject& object) = 0;

    // Save rendering results to consumer
    virtual void SaveRenderingResults(RenderingConsumer& consumer) const = 0;
};

}  // namespace null_engine::generic
