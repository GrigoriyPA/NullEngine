#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <null_engine/renderer/rasterization/rasterizer.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

class TextureRenderingConsumer {
public:
    explicit TextureRenderingConsumer(sf::Texture& texture);

    InPort<RasterizerBuffer>* GetTexturePort() const;

private:
    void OnRenderedTexture(const RasterizerBuffer& texture);

    sf::Texture& texture_;
    InPort<RasterizerBuffer>::Ptr in_texture_port_ = InPort<RasterizerBuffer>::Make();
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
