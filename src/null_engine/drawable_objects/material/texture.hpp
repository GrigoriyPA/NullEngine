#pragma once

#include <boost/compute/image.hpp>
#include <filesystem>
#include <memory>
#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/util/geometry/vector.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>
#include <optional>
#include <vector>

namespace null_engine {

class Texture {
public:
    using Ptr = std::unique_ptr<Texture>;
    using Buffer = boost::compute::image2d;

    Texture(uint64_t width, uint64_t height, const std::vector<Vec4>& colors);

    Texture(uint64_t width, uint64_t height, const uint8_t* colors);

    Texture(const Texture& other) = delete;

    Texture& operator=(const Texture& other) = delete;

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;

    const Vec4* GetColors() const;

    const Buffer& GetDeviceBuffer() const;

    void ToDevice(multithread::AccelerationContext context);

    static Texture::Ptr Monotonic(Vec4 color);

    static Texture::Ptr LoadFromFile(const std::filesystem::path& file);

    static Texture::Ptr LoadFromMemory(const void* data, size_t size);

private:
    uint64_t width_;
    uint64_t height_;
    std::vector<Vec4> colors_;
    std::optional<boost::compute::image2d> image_buffer_;
};

template <typename T>
class BufferView {
public:
    BufferView() = default;

    BufferView(uint64_t width, uint64_t height, const T* data, T outside_value)
        : width_(width)
        , height_(height)
        , data_(data)
        , outside_value_(outside_value) {
    }

    Vec2 GetTexelSize() const {
        return Vec2(1.0 / width_, 1.0 / height_);
    }

    T GetData(Vec2 position) const {
        assert(data_ && "Buffer is not initialized");

        const int64_t x = std::floor(position.x() * width_);
        const int64_t y = std::floor(position.y() * height_);

        if (0 <= x && x < width_ && 0 <= y && y < height_) {
            return data_[x + y * width_];
        }
        return outside_value_;
    }

protected:
    uint64_t width_ = 0;
    uint64_t height_ = 0;

private:
    const T* data_ = nullptr;
    T outside_value_;
};

class TextureView : private BufferView<Vec4> {
    using Base = BufferView<Vec4>;

public:
    explicit TextureView(const Texture& texture, Vec4 outside_color = kBlack);

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;

    Vec4 GetColor(Vec2 position) const;

    const Texture::Buffer& GetDeviceBuffer() const;

private:
    const Texture* texture_;
};

}  // namespace null_engine
