#pragma once

#include <boost/compute/image.hpp>
#include <filesystem>
#include <memory>
#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/util/geometry/vector.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>
#include <vector>

namespace null_engine {

class Texture {
public:
    using Ptr = std::unique_ptr<Texture>;
    using Buffer = boost::compute::image2d;

    Texture(uint64_t width, uint64_t height, const std::vector<Vec3>& colors);

    Texture(uint64_t width, uint64_t height, const uint8_t* colors);

    Texture(const Texture& other) = delete;

    Texture& operator=(const Texture& other) = delete;

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;

    const Vec3* GetColors() const;

    const Buffer& GetDeviceBuffer() const;

    void ToDevice(multithread::AccelerationContext context);

    static Texture::Ptr Monotonic(Vec3 color);

    static Texture::Ptr LoadFromFile(const std::filesystem::path& file);

    static Texture::Ptr LoadFromMemory(const void* data, size_t size);

private:
    uint64_t width_;
    uint64_t height_;
    std::vector<Vec3> colors_;
    std::optional<boost::compute::image2d> image_buffer_;
};

class TextureView {
public:
    explicit TextureView(const Texture& texture, Vec3 outside_color = kBlack);

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;

    Vec3 GetColor(Vec2 position) const;

    const Texture::Buffer& GetDeviceBuffer() const;

private:
    const Texture* texture_;
    uint64_t width_ = 0;
    uint64_t height_ = 0;
    const Vec3* colors_ = nullptr;
    Vec3 outside_color_ = kBlack;
};

}  // namespace null_engine
