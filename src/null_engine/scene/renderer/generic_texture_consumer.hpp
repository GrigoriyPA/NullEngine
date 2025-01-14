#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace null_engine {

class TextureRenderingConsumer {
public:
    explicit TextureRenderingConsumer(sf::Texture& texture);

    void OnRenderedTexture(const std::vector<uint8_t>& texture);

private:
    sf::Texture& texture_;
};

class WindowRenderingConsumer : public TextureRenderingConsumer, public sf::Drawable {
    using Base = TextureRenderingConsumer;

public:
    WindowRenderingConsumer(uint64_t width, uint64_t height);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};

}  // namespace null_engine
