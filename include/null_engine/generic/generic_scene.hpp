#pragma once

#include <null_engine/generic/camera/generic_camera.hpp>
#include <null_engine/generic/mesh/generic_mesh.hpp>
#include <null_engine/generic/renderer/generic_renderer.hpp>
#include <null_engine/generic/renderer/generic_rendering_consumer.hpp>

#include <vector>

namespace null_engine::generic {

//
// Graphic engine scene, contains all objects / lights / cameras
// After adding an object his structure freezing and cannot be changed
// Objects can be modified only by changing dynamics transforms
//
class Scene {
    struct RenderCamera {
        Camera camera;
        Renderer::Ptr renderer;
        RenderingConsumer::Ptr rendering_consumer;
    };

public:
    Scene() = default;

    Scene& SetDefaultRenderer(Renderer::Ptr default_renderer);
    Scene& SetDefaultRenderingConsumer(RenderingConsumer::Ptr default_rendering_consumer);

    Scene& AddObject(MeshObject::Ptr object);

    Scene& AddCamera(Camera camera);  // Requires default renderer / consumer
    Scene& AddCamera(Camera camera, Renderer::Ptr renderer, RenderingConsumer::Ptr rendering_consumer);

private:
    Renderer::Ptr default_renderer_ = nullptr;
    RenderingConsumer::Ptr default_rendering_consumer_ = nullptr;

    std::vector<MeshObject::Ptr> objects_;
    std::vector<RenderCamera> cameras_;
};

}  // namespace null_engine::generic
