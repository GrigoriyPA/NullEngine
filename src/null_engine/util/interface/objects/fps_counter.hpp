#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/interface/helpers/fonts.hpp>
#include <null_engine/util/interface/helpers/timer.hpp>

namespace null_engine::util {

class FPSCounter : public util::TimerClientBase<FPSCounter>, public sf::Drawable {
public:
    FPSCounter(FloatType update_period, const sf::Font& font, TimerProviderRef timer);

    FPSCounter& SetPosition(sf::Vector2f position);

    FPSCounter& SetFontSize(uint32_t font_size);

    FPSCounter& SetTextColor(sf::Color color);

    void Update(FloatType delta_time);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void SetFPS(FloatType fps);

private:
    const FloatType update_period_;
    const sf::Font& font_;

    sf::Text text_;
    FloatType spent_time_ = 0.0;
    uint64_t number_flips_ = 0;
};

}  // namespace null_engine::util
