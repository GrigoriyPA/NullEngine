#pragma once

#include <SFML/Graphics/Text.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

struct FPSDisplaySettings {
    FloatType update_period = 0.5;
    uint32_t font_size = 20;
    sf::Color text_color = sf::Color(255, 255, 255);
};

class FPSDisplay : public sf::Drawable {
public:
    FPSDisplay(FPSDisplaySettings settings, const sf::Font& font);

    InPort<FloatType>* GetRefreshPort() const;

    FPSDisplay& SetPosition(sf::Vector2f position);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void OnRefresh(FloatType delta_time);

    void SetFPS(FloatType fps);

private:
    FloatType update_period_;
    sf::Text display_text_;
    FloatType spent_time_ = 0.0;
    uint64_t number_flips_ = 0;
    InPort<FloatType>::Ptr in_refresh_port_ = InPort<FloatType>::Make();
};

}  // namespace null_engine
