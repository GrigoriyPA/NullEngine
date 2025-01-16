#include "texture_rendering_consumer.hpp"

#include <fmt/core.h>

#include <SFML/Graphics/RenderTarget.hpp>
#include <null_engine/util/generic/validation.hpp>

namespace null_engine {

TextureRenderingConsumer::TextureRenderingConsumer(sf::Texture& texture)
    : texture_(texture)
    , in_texture_port_(std::bind(&TextureRenderingConsumer::OnRenderedTexture, this, std::placeholders::_1)) {
}

InPort<TextureRenderingConsumer::TextureData>* TextureRenderingConsumer::GetTexturePort() {
    return &in_texture_port_;
}

void TextureRenderingConsumer::OnRenderedTexture(const TextureData& texture) {
    if (!texture.empty()) {
        texture_.update(texture.data());
    }
}

WindowRenderingConsumer::WindowRenderingConsumer(uint64_t width, uint64_t height)
    : Base(texture_) {
    Ensure(
        texture_.create(width, height),
        fmt::format("Failed to create window rendering consumer with texture size ({}, {})", width, height)
    );

    sprite_.setTexture(texture_);
}

void WindowRenderingConsumer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite_, states);
}

}  // namespace null_engine
