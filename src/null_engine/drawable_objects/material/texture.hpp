#pragma once

#include <memory>
#include <null_engine/util/geometry/vector.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>
#include <string>
#include <vector>

namespace null_engine {

class Texture {
public:
    using Ptr = std::unique_ptr<Texture>;

    Texture(uint64_t width, uint64_t height, const std::vector<Vec3>& colors);

    Texture(uint64_t width, uint64_t height, const uint8_t* colors);

    Texture(const Texture& other) = delete;

    Texture& operator=(const Texture& other) = delete;

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;

    const Vec3* GetColors() const;

    static Texture::Ptr Monotonic(Vec3 color);

    static Texture::Ptr LoadFromFile(const std::string& file);

private:
    uint64_t width_;
    uint64_t height_;
    std::vector<Vec3> colors_;
};

class TextureView {
public:
    TextureView() = default;

    explicit TextureView(const Texture& texture, Vec3 outside_color = kBlack);

    bool HasTexture() const;

    Vec3 GetColor(Vec2 position) const;

private:
    uint64_t width_ = 0;
    uint64_t height_ = 0;
    const Vec3* colors_ = nullptr;
    Vec3 outside_color_ = kBlack;
};

}  // namespace null_engine
