#pragma once

#include "generic_rendering_consumer.hpp"

#include <null_engine/util/generic/templates.hpp>
#include <null_engine/util/interface/objects/interface_object.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace null_engine::generic {

//
// Base class for SFML texture rendering consumers
//
class TextureRenderingConsumerBase : public RenderingConsumer {
public:
    explicit TextureRenderingConsumerBase(sf::Texture& texture);

public:
    void OnRenderedTexture(const std::vector<uint8_t>& texture) override;

private:
    sf::Texture& texture_;
};

//
// Saves rendering texture into SFML texture
//
class TextureRenderingConsumer : public TextureRenderingConsumerBase,
                                 public util::SharedConstructable<TextureRenderingConsumer> {
public:
    using Ptr = std::shared_ptr<TextureRenderingConsumer>;
    using Base = TextureRenderingConsumerBase;

    using Base::Base;
};

//
// Draw rendering texture on SFML window
//
class WindowRenderingConsumer : public TextureRenderingConsumerBase,
                                public util::InterfaceObject,
                                public util::SharedConstructable<WindowRenderingConsumer> {
public:
    using Ptr = std::shared_ptr<WindowRenderingConsumer>;
    using Base = TextureRenderingConsumerBase;

public:
    WindowRenderingConsumer(uint64_t width, uint64_t height);

public:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};

}  // namespace null_engine::generic
