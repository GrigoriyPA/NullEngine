#include "texture.hpp"

#include <CL/cl_platform.h>
#include <fmt/format.h>

#include <SFML/Graphics/Image.hpp>
#include <cassert>
#include <filesystem>
#include <null_engine/util/generic/validation.hpp>
#include <vector>

#include "null_engine/util/generic/types.hpp"

namespace null_engine {

namespace compute = boost::compute;

Texture::Texture(Width width, Height height, const std::vector<Vec4>& colors)
    : width_(width)
    , height_(height)
    , colors_(colors) {
    assert(colors_.size() == width_ * height_ && "Invalid texture initialization colors size");
}

Texture::Texture(Width width, Height height, const uint8_t* colors)
    : width_(width)
    , height_(height)
    , colors_(width_ * height_) {
    for (size_t i = 0; i < colors_.size(); ++i) {
        colors_[i] = Vec4(colors[4 * i], colors[4 * i + 1], colors[4 * i + 2], colors[4 * i + 3]) / 255.0;
    }
}

uint64_t Texture::GetWidth() const {
    return width_;
}

uint64_t Texture::GetHeight() const {
    return height_;
}

const Vec4* Texture::GetColors() const {
    return colors_.data();
}

const Texture::Buffer& Texture::GetDeviceBuffer() const {
    assert(image_buffer_ && "Compute image buffer was not initialized");
    return *image_buffer_;
}

void Texture::ToDevice(multithread::AccelerationContext context) {
    assert(!image_buffer_ && "Texture already attached to device");

    std::vector<cl_float> pixels(width_ * height_ * 4, 1.0);
    for (size_t i = 0; i < colors_.size(); ++i) {
        pixels[4 * i] = colors_[i].x();
        pixels[4 * i + 1] = colors_[i].y();
        pixels[4 * i + 2] = colors_[i].z();
        pixels[4 * i + 3] = colors_[i].w();
    }

    const compute::image_format format(CL_RGBA, CL_FLOAT);
    image_buffer_ = compute::image2d(context.GetContext(), width_, height_, format);

    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {width_, height_, 1};

    auto queue = context.GetQueue();
    queue.enqueue_write_image(*image_buffer_, origin, region, pixels.data(), 0, 0);
}

Texture::Uptr Texture::Monotonic(Vec4 color) {
    return std::make_unique<Texture>(Width{1}, Height{1}, std::vector{color});
}

Texture::Uptr Texture::LoadFromFile(const std::filesystem::path& file) {
    sf::Image image;
    Ensure(image.loadFromFile(file.string()), fmt::format("Failed to load texture from file {}", file.string()));

    return std::make_unique<Texture>(Width{image.getSize().x}, Height{image.getSize().y}, image.getPixelsPtr());
}

Texture::Uptr Texture::LoadFromMemory(const void* data, size_t size) {
    sf::Image image;
    Ensure(image.loadFromMemory(data, size), "Failed to load texture from memory");

    return std::make_unique<Texture>(Width{image.getSize().x}, Height{image.getSize().y}, image.getPixelsPtr());
}

TextureView::TextureView(const Texture& texture, Vec4 outside_color)
    : Base(Width{texture.GetWidth()}, Height{texture.GetHeight()}, texture.GetColors(), outside_color)
    , texture_(&texture) {
}

uint64_t TextureView::GetWidth() const {
    return width_;
}

uint64_t TextureView::GetHeight() const {
    return height_;
}

Vec4 TextureView::GetColor(Vec2 position) const {
    return GetData(position);
}

const Texture::Buffer& TextureView::GetDeviceBuffer() const {
    return texture_->GetDeviceBuffer();
}

}  // namespace null_engine
