#pragma once

#include <geometry/vector_3d.hpp>

namespace null_engine::native {

//
// One vertex of complex shape,
// Contains properties which can be interpolated
//
class Vertex {
public:
    Vertex(util::Vec3 position, util::Vec3 color);

    util::Vec3 GetPosition() const;
    util::Vec3 GetColor() const;

private:
    util::Vec3 position_;
    util::Vec3 color_;
};

}  // namespace null_engine::native
