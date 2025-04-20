#include "texture.hpp"

#include <CL/cl_platform.h>
#include <fmt/format.h>

#include <SFML/Graphics/Image.hpp>
#include <cassert>
#include <filesystem>
#include <null_engine/util/generic/validation.hpp>
#include <vector>

namespace null_engine {

namespace compute = boost::compute;

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
    }

    const compute::image_format format(CL_RGBA, CL_FLOAT);
    image_buffer_ = compute::image2d(context.GetContext(), width_, height_, format);

    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {width_, height_, 1};

    auto queue = context.GetQueue();
    queue.enqueue_write_image(*image_buffer_, origin, region, pixels.data(), 0, 0);
}

Texture::Ptr Texture::Monotonic(Vec3 color) {
    return std::make_unique<Texture>(1, 1, std::vector{color});
}

Texture::Ptr Texture::LoadFromFile(const std::filesystem::path& file) {
    sf::Image image;
    Ensure(image.loadFromFile(file.string()), fmt::format("Failed to load texture from file {}", file.string()));

    return std::make_unique<Texture>(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}

Texture::Ptr Texture::LoadFromMemory(const void* data, size_t size) {
    sf::Image image;
    Ensure(image.loadFromMemory(data, size), "Failed to load texture from memory");

    return std::make_unique<Texture>(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}

TextureView::TextureView(const Texture& texture, Vec3 outside_color)
    : Base(texture.GetWidth(), texture.GetHeight(), texture.GetColors(), outside_color)
    , texture_(&texture) {
}

uint64_t TextureView::GetWidth() const {
    return width_;
}

uint64_t TextureView::GetHeight() const {
    return height_;
}

Vec3 TextureView::GetColor(Vec2 position) const {
    return GetData(position);
}

const Texture::Buffer& TextureView::GetDeviceBuffer() const {
    return texture_->GetDeviceBuffer();
}

}  // namespace null_engine
