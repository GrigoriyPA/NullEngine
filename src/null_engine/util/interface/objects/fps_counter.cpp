#include "fps_counter.hpp"

#include <fmt/core.h>

#include <SFML/Graphics/RenderTarget.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>

namespace null_engine {

FPSCounter::FPSCounter(FloatType update_period, const sf::Font& font, TimerProviderRef timer)
    : TimerClientBase<FPSCounter>(timer)
    , update_period_(update_period)
    , font_(font)
    , spent_time_(0.0)
    , number_flips_(0) {
    text_.setFont(font);
    text_.setFillColor(kDefaultTextColor);
    text_.setCharacterSize(kDefaultFontSize);

    SetFPS(0);
}

FPSCounter& FPSCounter::SetPosition(sf::Vector2f position) {
    text_.setPosition(position);
    return *this;
}

FPSCounter& FPSCounter::SetFontSize(uint32_t font_size) {
    text_.setCharacterSize(font_size);
    return *this;
}

FPSCounter& FPSCounter::SetTextColor(sf::Color color) {
    text_.setFillColor(color);
    return *this;
}

void FPSCounter::Update(FloatType delta_time) {
    spent_time_ += delta_time;
    number_flips_++;

    if (spent_time_ >= update_period_) {
        SetFPS(static_cast<FloatType>(number_flips_) / spent_time_);
        spent_time_ = 0.0;
        number_flips_ = 0;
    }
}

void FPSCounter::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(text_, states);
}

void FPSCounter::SetFPS(FloatType fps) {
    text_.setString(fmt::format("FPS: {:.3f}", fps));
}

}  // namespace null_engine
