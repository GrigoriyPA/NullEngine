#pragma once

#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine {

class Vertex {
public:
    Vertex(Vec3 position, Vec3 color);

    Vec3 GetPosition() const;

    Vec3 GetColor() const;

    Vertex& SetPosition(Vec3 position);

    Vertex& SetColor(Vec3 color);

private:
    Vec3 position_;
    Vec3 color_;
};

}  // namespace null_engine
