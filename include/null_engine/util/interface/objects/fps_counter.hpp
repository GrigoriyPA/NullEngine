#pragma once

#include "interface_object.hpp"

#include <null_engine/util/generic/templates.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/interface/helpers/fonts.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

namespace null_engine::util {

//
// Show average FPS for given update period
//
class FPSCounter : public InterfaceObject, public SharedConstructable<FPSCounter> {
public:
    using Ptr = std::shared_ptr<FPSCounter>;

public:
    FPSCounter(FloatType update_period, FontPtr font);

    FPSCounter& SetPosition(sf::Vector2f position);
    FPSCounter& SetFontSize(uint32_t font_size);
    FPSCounter& SetTextColor(sf::Color color);

public:
    void Update(FloatType delta_time) override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void SetFPS(FloatType fps);

private:
    const FloatType update_period_;
    const FontPtr font_;

    sf::Text text_;
    FloatType spent_time_ = 0.0;
    uint64_t number_flips_ = 0;
};

}  // namespace null_engine::util
