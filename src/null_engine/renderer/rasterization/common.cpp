#include "common.hpp"

namespace null_engine::detail {

void PerspectiveDivision(Vec4& position) {
    position.x() /= position.w();
    position.y() /= position.w();
    position.z() /= position.w();
    position.w() = 1.0 / position.w();
}

}  // namespace null_engine::detail
