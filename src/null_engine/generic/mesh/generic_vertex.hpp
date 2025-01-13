#pragma once

#include <null_engine/util/geometry/transformation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

namespace null_engine::generic {

class Vertex {
public:
    Vertex(util::Vec3 position, util::Vec3 color);

    util::Vec3 GetPosition() const;

    util::Vec3 GetColor() const;

    Vertex& SetPosition(util::Vec3 position);

    Vertex& SetColor(util::Vec3 color);

    void ApplyNdcTransform(const util::Transform& ndc_transform);

private:
    util::Vec3 position_;
    util::Vec3 color_;
};

}  // namespace null_engine::generic
