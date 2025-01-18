#include "primitive_objects.hpp"

namespace null_engine {

VerticesObject CreateNormalsVisualization(const VerticesObject& object, Vec3 color, FloatType scale) {
    VerticesObject result(2 * object.GetNumberVertices(), VerticesObject::Type::Lines);

    for (uint64_t i = 0; const auto& vertex : object.GetVertices()) {
        result.SetVertex(i++, {.position = vertex.position, .params = {.color = color}});
        result.SetVertex(i++, {.position = vertex.position + vertex.params.normal * scale, .params = {.color = color}});
    }

    return result;
}

}  // namespace null_engine
