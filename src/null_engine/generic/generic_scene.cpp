#include "generic_scene.hpp"

#include <null_engine/util/generic/validation.hpp>

namespace null_engine::generic {

CameraView::CameraView(Camera::Ptr camera, Renderer::Ptr renderer)
    : camera_(camera)
    , renderer_(renderer) {
}

Camera& CameraView::GetCamera() const {
    return *camera_;
}

Renderer& CameraView::GetRenderer() const {
    return *renderer_;
}

const std::vector<RenderingConsumer::Ptr>& CameraView::GetConsumers() const {
    return rendering_consumers_;
}

CameraView& CameraView::AddConsumer(RenderingConsumer::Ptr rendering_consumer) {
    rendering_consumers_.emplace_back(std::move(rendering_consumer));
    return *this;
}

Scene& Scene::SetDefaultRenderer(Renderer::Ptr default_renderer) {
    default_renderer_ = std::move(default_renderer);
    return *this;
}

Scene& Scene::AddObject(MeshObject::Ptr object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

CameraView& Scene::AddCamera(Camera::Ptr camera) {
    util::Ensure(default_renderer_ != nullptr, "Default renderer is required for adding new camera into scene");

    return views_.emplace_back(std::move(camera), default_renderer_);
}

CameraView& Scene::AddCamera(Camera::Ptr camera, Renderer::Ptr renderer) {
    return views_.emplace_back(std::move(camera), std::move(renderer));
}

void Scene::Render() const {
    util::Ensure(!views_.empty(), "Cannot render scene without any view cameras");

    for (const auto& view : views_) {
        auto& renderer = view.GetRenderer();
        renderer.SetCamera(view.GetCamera());  // Reset rendering context

        // Draw all objects
        for (const auto& object : objects_) {
            renderer.RenderObject(*object);
        }

        // Pass rendered data to consumers
        for (const auto& consumer : view.GetConsumers()) {
            renderer.SaveRenderingResults(*consumer);
        }
    }
}

}  // namespace null_engine::generic
