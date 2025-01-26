#include "view.hpp"

#include <null_engine/util/interface/helpers/fonts.hpp>
#include <null_engine/util/interface/objects/fps_display.hpp>
#include <null_engine/util/interface/objects/texture_rendering_consumer.hpp>

#include "events.hpp"

namespace null_engine::tests {

namespace {

const sf::Color kBackgroundColor = sf::Color(100, 100, 100);
constexpr const char* kFontPath = "../../assets/fonts/arial.ttf";

ViewAssetes LoadAssets() {
    return {.font = LoadFont(kFontPath)};
}

}  // anonymous namespace

View::View(sf::RenderWindow& window)
    : window_(window)
    , assetes_(LoadAssets())
    , in_draw_event_port_(std::bind(&View::OnDrawEvent, this, std::placeholders::_1)) {
    auto renderer_view = std::make_unique<native::WindowRenderingConsumer>(window_.getSize().x, window_.getSize().y);
    out_texture_port_->Subscribe(renderer_view->GetTexturePort(), {});
    interface_.AddObject(std::move(renderer_view));

    auto fps_display = std::make_unique<FPSDisplay>(FPSDisplaySettings{}, *assetes_.font);
    out_refresh_port_->Subscribe(fps_display->GetRefreshPort(), 0.0);
    interface_.AddObject(std::move(fps_display));

    out_texture_id_port_->Subscribe(render_view_.GetTexturePort(), 0);
}

InPort<DrawViewEvent>* View::GetDrawEventsPort() {
    return &in_draw_event_port_;
}

void View::OnDrawEvent(const DrawViewEvent& draw_event) const {
    out_refresh_port_->Notify(draw_event.delta_time);

    window_.clear(kBackgroundColor);

    if (std::holds_alternative<TextureData>(draw_event.render_texture)) {
        out_texture_port_->Notify(std::get<TextureData>(draw_event.render_texture));
    } else {
        out_texture_id_port_->Notify(std::get<GLuint>(draw_event.render_texture));
    }

    window_.pushGLStates();
    window_.draw(interface_);
    window_.popGLStates();

    window_.display();
}

}  // namespace null_engine::tests
