#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/util/interface/gui_interface.hpp>
#include <null_engine/util/mvc/ports.hpp>

#include "events.hpp"

namespace null_engine::tests {

class View {
public:
    explicit View(sf::RenderWindow& window);

    InPort<DrawViewEvent>* GetDrawEventsPort();

private:
    void OnDrawEvent(const DrawViewEvent& draw_event) const;

    sf::RenderWindow& window_;
    sf::Font font_;
    GuiInterface interface_;
    InPort<DrawViewEvent> in_draw_event_port_;
    OutPort<FloatType>::Ptr out_refresh_port_ = OutPort<FloatType>::Make();
    OutPort<TextureData>::Ptr out_texture_port_ = OutPort<TextureData>::Make();
};

}  // namespace null_engine::tests
