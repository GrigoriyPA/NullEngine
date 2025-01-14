#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <null_engine/renderer/rasterization/rasterizer.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

class TextureRenderingConsumer {
public:
    using TextureData = std::vector<uint8_t>;

    explicit TextureRenderingConsumer(sf::Texture& texture);

    InPort<TextureData>* GetTexturePort() const;

private:
    void OnRenderedTexture(const TextureData& texture);

    sf::Texture& texture_;
    InPort<TextureData>::Ptr in_texture_port_;
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
