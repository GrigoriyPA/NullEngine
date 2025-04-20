#pragma once

#include <null_engine/drawable_objects/material/texture.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/matrix.hpp>

namespace null_engine::detail {

struct ShadowInfo {
    uint64_t shadow_width;
    uint64_t shadow_height;
    ProjectiveTransform transform;
    Vec3 light_pos;
};

class DepthBuffer : public BufferView<FloatType> {
    using Base = BufferView<FloatType>;

public:
    DepthBuffer() = default;

    DepthBuffer(const ShadowInfo& shadow_info, const FloatType* depth);
};

}  // namespace null_engine::detail
