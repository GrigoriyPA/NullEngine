#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/util/interface/gui_interface.hpp>
#include <null_engine/util/interface/objects/texture_rendering_consumer.hpp>
#include <null_engine/util/observer/ports.hpp>

#include "common.hpp"

namespace null_engine::example {

struct ViewAssetes {
    std::unique_ptr<sf::Font> font;
};

class View {
    using TextureRenderingConsumer = multithread::TextureRenderingConsumer;

public:
    explicit View(sf::RenderWindow& window);

    InPort<DrawViewEvent>* GetDrawEventsPort();

private:
    void OnDrawEvent(const DrawViewEvent& draw_event) const;

    sf::RenderWindow& window_;
    ViewAssetes assetes_;
    GuiInterface interface_;
    TextureRenderingConsumer render_view_;
    InPort<DrawViewEvent> in_draw_event_port_;
    OutPort<float>::Uptr out_refresh_port_ = OutPort<float>::Make();
    OutPort<TextureData>::Uptr out_texture_port_ = OutPort<TextureData>::Make();
    OutPort<GLuint>::Uptr out_texture_id_port_ = OutPort<GLuint>::Make();
};

}  // namespace null_engine::example
