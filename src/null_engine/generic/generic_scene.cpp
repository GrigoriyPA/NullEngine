#include <null_engine/generic/generic_scene.hpp>

#include <null_engine/util/generic/validation.hpp>

namespace null_engine::generic {

//// Scene

Scene& Scene::SetDefaultRenderer(Renderer::Ptr default_renderer) {
    default_renderer_ = std::move(default_renderer);
    return *this;
}

Scene& Scene::SetDefaultRenderingConsumer(RenderingConsumer::Ptr default_rendering_consumer) {
    default_rendering_consumer_ = std::move(default_rendering_consumer);
    return *this;
}

Scene& Scene::AddObject(MeshObject::Ptr object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

Scene& Scene::AddCamera(Camera camera) {
    util::Ensure(default_renderer_ != nullptr, "Default renderer is required for adding new camera into scene");
    util::Ensure(default_rendering_consumer_ != nullptr,
                 "Default rendering consumer is required for adding new camera into scene");

    cameras_.emplace_back(RenderCamera{
        .camera = std::move(camera), .renderer = default_renderer_, .rendering_consumer = default_rendering_consumer_});
    return *this;
}

Scene& Scene::AddCamera(Camera camera, Renderer::Ptr renderer, RenderingConsumer::Ptr rendering_consumer) {
    cameras_.emplace_back(RenderCamera{.camera = std::move(camera),
                                       .renderer = std::move(renderer),
                                       .rendering_consumer = std::move(rendering_consumer)});
    return *this;
}

}  // namespace null_engine::generic
