#include "model.hpp"

#include <iostream>

namespace null_engine::tests {

Model::Model(uint64_t view_width, uint64_t view_height, bool multithread_rendering)
    : acceleration_context_(
          multithread_rendering ? std::optional<AccelerationContext>(AccelerationContext::Create()) : std::nullopt
      )
    , scene_info_(SceneInfo::LoadMjolnir(
          {.view_width = view_width, .view_height = view_height, .acceleration_context = acceleration_context_}
      ))
    , native_renderer_({view_width, view_height})
    , in_texture_port_(std::bind(&Model::OnNativeRenderedTexture, this, std::placeholders::_1))
    , in_texture_id_port_(std::bind(&Model::OnMultithreadRenderedTexture, this, std::placeholders::_1)) {
    native_renderer_.SubscribeToTextures(&in_texture_port_);

    if (multithread_rendering) {
        multithread_renderer_ =
            std::make_unique<MultithreadRenderer>(RendererSettings{view_width, view_height}, *acceleration_context_);
        multithread_renderer_->SubscribeToTextures(&in_texture_id_port_);

        std::cout << "Discovered device:\n" << acceleration_context_->GetDeviceDescription() << "\n";
    }
}

void Model::SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const {
    DrawViewEvent event = {.delta_time = current_delta_time_};
    if (multithread_renderer_) {
        event.render_texture = current_texture_id_;
    } else {
        event.render_texture = current_texture_;
    }

    out_draw_event_port_->Subscribe(observer_port, event);
}

void Model::DoRendering() {
    if (multithread_renderer_) {
        multithread_renderer_->GetRenderPort()->OnEvent({
            .scene = scene_info_->GetScene(),
            .camera = scene_info_->GetCamera(),
        });
    } else {
        native_renderer_.GetRenderPort()->OnEvent({
            .scene = scene_info_->GetScene(),
            .camera = scene_info_->GetCamera(),
        });
    }
}

void Model::MoveCamera(const CameraChange& camera_change) {
    scene_info_->OnCameraEvent(camera_change);
}

void Model::Refresh(FloatType delta_time) {
    current_delta_time_ = delta_time;
    scene_info_->OnRefreshEvent(delta_time);
}

void Model::OnNativeRenderedTexture(const TextureData& texture) {
    current_texture_ = texture;
    out_draw_event_port_->Notify({.delta_time = current_delta_time_, .render_texture = current_texture_});
}

void Model::OnMultithreadRenderedTexture(GLuint texture_id) {
    current_texture_id_ = texture_id;
    out_draw_event_port_->Notify({.delta_time = current_delta_time_, .render_texture = current_texture_id_});
}

}  // namespace null_engine::tests
