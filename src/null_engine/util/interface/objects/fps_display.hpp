#pragma once

#include <SFML/Graphics/Text.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/observer/ports.hpp>

namespace null_engine {

struct FPSDisplaySettings {
    float update_period = 0.5;
    uint32_t font_size = 20;
    sf::Color text_color = sf::Color(255, 255, 255);
};

class FPSDisplay : public sf::Drawable {
public:
    FPSDisplay(FPSDisplaySettings settings, const sf::Font& font);

    InPort<float>* GetRefreshPort();

    FPSDisplay& SetPosition(sf::Vector2f position);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void OnRefresh(float delta_time);

    void SetFPS(float fps);

    float update_period_;
    sf::Text display_text_;
    float spent_time_ = 0.0;
    uint64_t number_flips_ = 0;
    InPort<float> in_refresh_port_;
};

}  // namespace null_engine
