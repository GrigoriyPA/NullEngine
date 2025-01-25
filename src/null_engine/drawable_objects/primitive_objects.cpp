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

    const std::vector<Vec2> cube_face_tex_coords = {
        {Vec2(1.0, 1.0)},
        {Vec2(0.0, 1.0)},
        {Vec2(0.0, 0.0)},
        {Vec2(1.0, 0.0)},
    };

    const std::vector<uint64_t> cube_face_inices = {0, 1, 2, 2, 3, 0};

    auto cube_face = VerticesObject(4, VerticesObject::Type::Triangles)
                         .SetPositions(cube_face_positions)
                         .SetTexCoords(cube_face_tex_coords)
                         .SetIndices(cube_face_inices)
                         .GenerateNormals();

    auto cube = cube_face;

    const auto y_axis = Vec3(0.0, 1.0, 0.0);
    const auto angle = std::numbers::pi / 2.0;
    const auto y_rotation = Rotation(y_axis, angle);
    cube.Merge(cube_face.ApplyTransform(y_rotation));
    cube.Merge(cube_face.ApplyTransform(y_rotation));
    cube.Merge(cube_face.ApplyTransform(y_rotation));

    const auto z_axis = Vec3(0.0, 0.0, 1.0);
    const auto z_rotation = Rotation(z_axis, angle);
    cube.Merge(cube_face.ApplyTransform(z_rotation));
    cube.Merge(cube_face.ApplyTransform(z_rotation * z_rotation));

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

VerticesObject CreateDirectLightVisualization(Vec3 color) {
    const uint64_t number_vertices = 6;
    VerticesObject result(number_vertices, VerticesObject::Type::Lines);

    const Vec3 direction(0.0, 0.0, 1.0);
    const Vec3 horizon(1.0, 0.0, 0.0);
    const Vec3 vertical(0.0, 1.0, 0.0);

    result.SetPositions({
        horizon + vertical,
        -horizon - vertical,
        -horizon + vertical,
        horizon - vertical,
        Vec3(0.0, 0.0, 0.0),
        2.0 * direction,
    });
    result.SetColors(color);

    return result;
}

VerticesObject CreatePointLightVisualization(Vec3 color) {
    const uint64_t number_vertices = 6;
    VerticesObject result(number_vertices, VerticesObject::Type::Lines);

    const Vec3 x_axis(1.0, 0.0, 0.0);
    const Vec3 y_axis(0.0, 1.0, 0.0);
    const Vec3 z_axis(0.0, 0.0, 1.0);

    result.SetPositions({x_axis, -x_axis, y_axis, -y_axis, z_axis, -z_axis});
    result.SetColors(color);

    const auto rotation_angle = std::numbers::pi / 4.0;
    const auto y_rotation = Rotation(y_axis, rotation_angle);
    const auto z_rotation = Rotation(z_axis, rotation_angle);
    result.Merge(VerticesObject(result).ApplyTransform(z_rotation * y_rotation));

    return result;
}

}  // namespace null_engine
