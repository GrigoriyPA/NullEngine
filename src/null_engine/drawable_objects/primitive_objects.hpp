#pragma once

#include "vertices_object.hpp"

namespace null_engine {

VerticesObject CreateQuad(bool generate_normals);

VerticesObject CreateCube();

VerticesObject CreateNormalsVisualization(const VerticesObject& object, Vec4 color, float scale = 1.0);

VerticesObject CreateDirectLightVisualization(Vec4 color);

VerticesObject CreatePointLightVisualization(Vec4 color);

}  // namespace null_engine
