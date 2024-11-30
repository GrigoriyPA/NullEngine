#include <null_engine/generic/renderer/generic_texture_consumer.hpp>

namespace null_engine::generic {

//// TextureRenderingConsumer

TextureRenderingConsumer::TextureRenderingConsumer(sf::Texture& texture)
    : texture_(texture) {
}

void TextureRenderingConsumer::OnRenderedTexture(const std::vector<uint8_t>& texture) {
    texture_.update(texture.data());
}

}  // namespace null_engine::generic
