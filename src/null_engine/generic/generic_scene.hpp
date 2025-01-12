#pragma once

#include <null_engine/generic/camera/generic_camera.hpp>
#include <null_engine/generic/mesh/generic_mesh.hpp>
#include <null_engine/generic/renderer/generic_renderer.hpp>
#include <null_engine/generic/renderer/generic_rendering_consumer.hpp>
#include <vector>

namespace null_engine::generic {

//
// Rendering unit, contains camera, randerer and set of consumers.
// Same renderer may be reused in different cameras
//
class CameraView {
public:
    CameraView(Camera::Ptr camera, Renderer::Ptr renderer);

    Camera& GetCamera() const;
    Renderer& GetRenderer() const;
    const std::vector<RenderingConsumer::Ptr>& GetConsumers() const;

    CameraView& AddConsumer(RenderingConsumer::Ptr rendering_consumer);

private:
    Camera::Ptr camera_;
    Renderer::Ptr renderer_;
    std::vector<RenderingConsumer::Ptr> rendering_consumers_;
};

//
// Graphic engine scene, contains all objects / lights / cameras
// After adding an object his structure freezing and cannot be changed
// Objects can be modified only by changing dynamics transforms
//
class Scene {
public:
    Scene() = default;

    Scene& SetDefaultRenderer(Renderer::Ptr default_renderer);

    Scene& AddObject(MeshObject::Ptr object);

    CameraView& AddCamera(Camera::Ptr camera);
    CameraView& AddCamera(Camera::Ptr camera, Renderer::Ptr renderer);
    Scene& AddCamera(CameraView camera_view);

public:
    void Render() const;

private:
    Renderer::Ptr default_renderer_ = nullptr;

    std::vector<MeshObject::Ptr> objects_;
    std::vector<CameraView> views_;
};

}  // namespace null_engine::generic
