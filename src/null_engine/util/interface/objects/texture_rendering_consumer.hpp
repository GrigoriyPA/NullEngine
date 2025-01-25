#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/OpenGL.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

namespace native {

class TextureRenderingConsumer {
public:
    using TextureData = std::vector<uint8_t>;

    explicit TextureRenderingConsumer(sf::Texture& texture);

    InPort<TextureData>* GetTexturePort();

private:
    void OnRenderedTexture(const TextureData& texture);

    sf::Texture& texture_;
    InPort<TextureData> in_texture_port_;
};

class WindowRenderingConsumer : public TextureRenderingConsumer, public sf::Drawable {
    using Base = TextureRenderingConsumer;

public:
    WindowRenderingConsumer(uint64_t width, uint64_t height);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};

}  // namespace native

namespace multithread {

class TextureRenderingConsumer {
public:
    TextureRenderingConsumer();

    InPort<GLuint>* GetTexturePort();

private:
    void OnRenderedTexture(GLuint texture_id) const;

    InPort<GLuint> in_texture_port_;
};

}  // namespace multithread

}  // namespace null_engine
