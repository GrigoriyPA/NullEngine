#include "view.hpp"

#include <null_engine/util/interface/helpers/fonts.hpp>

namespace null_engine::tests {

namespace {

constexpr const char* kFontPath = "../../assets/fonts/arial.ttf";

}  // anonymous namespace

View::View(sf::RenderWindow& window)
    : window_(window)
    , font_(LoadFont(kFontPath))
    , rendering_consumer_(window.getSize().x, window.getSize().y)
    , fps_display_({}, font_)
    , in_refresh_port_(InPort<FloatType>::Make(std::bind(&View::OnRefresh, this, std::placeholders::_1))) {
    out_refresh_port_->Subscribe(fps_display_.GetRefreshPort());
}

InPort<FloatType>* View::GetRefreshPort() const {
    return in_refresh_port_.get();
}

InPort<View::TextureData>* View::GetTexturePort() const {
    return rendering_consumer_.GetTexturePort();
}

void View::OnRefresh(FloatType delta_time) const {
    out_refresh_port_->Notify(delta_time);

    window_.clear();
    window_.draw(rendering_consumer_);
    window_.draw(fps_display_);
    window_.display();
}

}  // namespace null_engine::tests
