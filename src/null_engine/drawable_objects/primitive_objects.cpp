#include "primitive_objects.hpp"

#include <numbers>

namespace null_engine {

VerticesObject CreateCube() {
    const std::vector<Vec3> cube_face_positions = {
        Vec3(0.5, -0.5, -0.5),
        Vec3(-0.5, -0.5, -0.5),
        Vec3(-0.5, 0.5, -0.5),
        Vec3(0.5, 0.5, -0.5),
    };

    const std::vector<VertexParams> cube_face_params = {
        {.tex_coords = Vec2(1.0, 1.0)},
        {.tex_coords = Vec2(0.0, 1.0)},
        {.tex_coords = Vec2(0.0, 0.0)},
        {.tex_coords = Vec2(1.0, 0.0)},
    };

    const std::vector<uint64_t> cube_face_inices = {0, 1, 2, 2, 3, 0};

    auto cube_face = VerticesObject(4, VerticesObject::Type::Triangles)
                         .SetPositions(cube_face_positions)
                         .SetParams(cube_face_params)
                         .SetIndices(cube_face_inices)
                         .GenerateNormals();

    auto cube = cube_face;

    const auto y_axis = Vec3(0.0, 1.0, 0.0);
    const auto angle = std::numbers::pi / 2.0;
    const auto y_rotation = Mat4::Rotation(y_axis, angle);
    cube.Merge(cube_face.Transform(y_rotation));
    cube.Merge(cube_face.Transform(y_rotation));
    cube.Merge(cube_face.Transform(y_rotation));

    const auto z_axis = Vec3(0.0, 0.0, 1.0);
    const auto z_rotation = Mat4::Rotation(z_axis, angle);
    cube.Merge(cube_face.Transform(z_rotation));
    cube.Merge(cube_face.Transform(z_rotation * z_rotation));

    return cube;
}

VerticesObject CreateNormalsVisualization(const VerticesObject& object, Vec3 color, FloatType scale) {
    VerticesObject result(2 * object.GetNumberVertices(), VerticesObject::Type::Lines);

    for (uint64_t i = 0; const auto& vertex : object.GetVertices()) {
        result.SetVertex(i++, {.position = vertex.position, .params = {.color = color}});
        result.SetVertex(i++, {.position = vertex.position + vertex.params.normal * scale, .params = {.color = color}});
    }

    return result;
}

}  // namespace null_engine
