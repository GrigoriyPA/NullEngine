#pragma once

#include "vertices_object.hpp"

namespace null_engine {

VerticesObject CreateCube();

VerticesObject CreateNormalsVisualization(const VerticesObject& object, Vec3 color, FloatType scale = 1.0);

}  // namespace null_engine
