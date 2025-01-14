#include "fps_display.hpp"

#include <fmt/core.h>

#include <SFML/Graphics/RenderTarget.hpp>

namespace null_engine {

FPSDisplay::FPSDisplay(FPSDisplaySettings settings, const sf::Font& font)
    : update_period_(settings.update_period)
    , spent_time_(0.0)
    , number_flips_(0) {
    in_refresh_port_->SetEventsHandler(std::bind(&FPSDisplay::OnRefresh, this, std::placeholders::_1));

    display_text_.setFont(font);
    display_text_.setFillColor(settings.text_color);
    display_text_.setCharacterSize(settings.font_size);

    SetFPS(0);
}

InPort<FloatType>* FPSDisplay::GetRefreshPort() const {
    return in_refresh_port_.get();
}

FPSDisplay& FPSDisplay::SetPosition(sf::Vector2f position) {
    display_text_.setPosition(position);
    return *this;
}

void FPSDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(display_text_, states);
}

void FPSDisplay::OnRefresh(FloatType delta_time) {
    spent_time_ += delta_time;
    number_flips_++;

    if (spent_time_ >= update_period_) {
        SetFPS(static_cast<FloatType>(number_flips_) / spent_time_);
        spent_time_ = 0.0;
        number_flips_ = 0;
    }
}

void FPSDisplay::SetFPS(FloatType fps) {
    display_text_.setString(fmt::format("FPS: {:.3f}", fps));
}

}  // namespace null_engine
