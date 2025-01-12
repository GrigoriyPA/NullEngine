#include "generic_texture_consumer.hpp"

#include <fmt/core.h>

#include <SFML/Graphics/RenderTarget.hpp>
#include <null_engine/util/generic/validation.hpp>

namespace null_engine::generic {

TextureRenderingConsumer::TextureRenderingConsumer(sf::Texture& texture)
    : texture_(texture) {
}

void TextureRenderingConsumer::OnRenderedTexture(const std::vector<uint8_t>& texture) {
    texture_.update(texture.data());
}

WindowRenderingConsumer::WindowRenderingConsumer(uint64_t width, uint64_t height)
    : Base(texture_) {
    util::Ensure(
        texture_.create(width, height),
        fmt::format("Failed to create window rendering consumer with texture size ({}, {})", width, height)
    );

    sprite_.setTexture(texture_);
}

void WindowRenderingConsumer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite_, states);
}

}  // namespace null_engine::generic
