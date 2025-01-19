#pragma once

#include "vertices_object.hpp"

namespace null_engine {

VerticesObject CreateCube();

VerticesObject CreateNormalsVisualization(const VerticesObject& object, Vec3 color, FloatType scale = 1.0);

VerticesObject CreateDirectLightVisualization(Vec3 color);

VerticesObject CreatePointLightVisualization(Vec3 color);

}  // namespace null_engine
