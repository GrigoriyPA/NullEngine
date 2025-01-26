#pragma once

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/opengl_texture.hpp>
#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/renderer/shaders/vertex_shader.hpp>

namespace null_engine::multithread::detail {

struct RasterizerBuffer {
    compute::opengl_texture colors;
    compute::buffer depth;
};

class Rasterizer {
    using TriangleIndex = null_engine::detail::TriangleIndex;
    using InterpVertex = null_engine::detail::InterpVertex;

    struct VertexInfo {
        cl_float4 pos;
        cl_float3 color;
    };

public:
    Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context);

    void DrawTriangles(
        const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
    );

private:
    void FillVerticesInfo(const std::vector<InterpVertex>& points);

    cl_int2 view_size_;
    compute::context context_;
    compute::command_queue queue_;
    compute::program program_;
    compute::kernel kernel_;
    std::vector<VertexInfo> vertices_info_;
};

}  // namespace null_engine::multithread::detail
