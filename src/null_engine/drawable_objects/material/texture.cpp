#include "texture.hpp"

#include <SFML/Graphics/Image.hpp>
#include <cassert>

namespace null_engine {

Texture::Texture(uint64_t width, uint64_t height, const std::vector<Vec3>& colors)
    : width_(width)
    , height_(height)
    , colors_(colors) {
    assert(colors_.size() == width_ * height_ && "Invalid texture initialization colors size");
}

Texture::Texture(uint64_t width, uint64_t height, const uint8_t* colors)
    : width_(width)
    , height_(height)
    , colors_(width * height) {
    for (size_t i = 0; i < colors_.size(); ++i) {
        colors_[i] = Vec3(colors[4 * i], colors[4 * i + 1], colors[4 * i + 2]) / 255.0;
    }
}

uint64_t Texture::GetWidth() const {
    return width_;
}

uint64_t Texture::GetHeight() const {
    return height_;
}

const Vec3* Texture::GetColors() const {
    return colors_.data();
}

Texture::Ptr Texture::Monotonic(Vec3 color) {
    return std::make_unique<Texture>(1, 1, std::vector{color});
}

Texture::Ptr Texture::LoadFromFile(const std::string& file) {
    sf::Image image;
    image.loadFromFile(file);

    return std::make_unique<Texture>(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}

TextureView::TextureView(const Texture& texture, Vec3 outside_color)
    : width_(texture.GetWidth())
    , height_(texture.GetHeight())
    , colors_(texture.GetColors())
    , outside_color_(outside_color) {
}

bool TextureView::HasTexture() const {
    return colors_ != nullptr;
}

Vec3 TextureView::GetColor(Vec2 position) const {
    position *= Vec2(width_, height_);

    const int64_t x = std::floor(position.X());
    const int64_t y = std::floor(position.Y());

    if (0 <= x && x < width_ && 0 <= y && y < height_) {
        return colors_[x + y * width_];
    }
    return outside_color_;
}

}  // namespace null_engine
