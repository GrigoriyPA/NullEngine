#pragma once

#include "generic_rendering_consumer.hpp"

#include <null_engine/util/generic/templates.hpp>

#include <SFML/Graphics/Texture.hpp>

namespace null_engine::generic {

//
// Saves rendering texture into SFML texture
//
class TextureRenderingConsumer : public RenderingConsumer, public util::SharedConstructable<TextureRenderingConsumer> {
public:
    using Ptr = std::shared_ptr<TextureRenderingConsumer>;

public:
    explicit TextureRenderingConsumer(sf::Texture& texture);

public:
    void OnRenderedTexture(const std::vector<uint8_t>& texture) override;

private:
    sf::Texture& texture_;
};

}  // namespace null_engine::generic
