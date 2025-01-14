#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/renderer/consumers/texture_rendering_consumer.hpp>
#include <null_engine/util/interface/fps_display.hpp>

namespace null_engine::tests {

class View {
public:
    using TextureData = std::vector<uint8_t>;

    explicit View(sf::RenderWindow& window);

    InPort<FloatType>* GetRefreshPort() const;

    InPort<TextureData>* GetTexturePort() const;

private:
    void OnRefresh(FloatType delta_time) const;

    sf::RenderWindow& window_;
    sf::Font font_;
    WindowRenderingConsumer rendering_consumer_;
    FPSDisplay fps_display_;
    InPort<FloatType>::Ptr in_refresh_port_;
    OutPort<FloatType>::Ptr out_refresh_port_ = OutPort<FloatType>::Make();
};

}  // namespace null_engine::tests
