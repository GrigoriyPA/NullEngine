#pragma once

#include <CL/cl_platform.h>

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/opengl_texture.hpp>
#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/renderer/shaders/multithread_fragment_shader.hpp>
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
        cl_float3 normal;
        cl_float2 tex_coords;
        cl_float3 frag_pos;
    };

public:
    static constexpr uint32_t kTrianglesInBatch = 200;

    struct WorkBatch {
        cl_int3 triangles[kTrianglesInBatch];
        cl_int number_traingles;
    };

public:
    Rasterizer(uint64_t view_width, uint64_t view_height, AccelerationContext context);

    void SetSceneInfo(const FragmentShader& shader, Vec3 view_pos, const std::vector<AnyLight>& lights);

    void SetMaterialInfo(const FragmentShader& shader, const Material& material);

    void DrawTriangles(
        const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
    );

private:
    void FillVerticesInfo(const std::vector<InterpVertex>& points);

    void FillWorkBatches(const std::vector<TriangleIndex>& indices);

    cl_int2 view_size_;
    cl_int2 work_size_;
    compute::context context_;
    compute::command_queue queue_;
    Program program_;
    compute::kernel kernel_;
    std::vector<VertexInfo> vertices_info_;
    compute::buffer vertices_info_buffer_;
    size_t number_works_ = 0;
    std::vector<size_t> batch_id_;
    std::vector<std::vector<WorkBatch>> work_batches_;
    compute::buffer work_batches_buffer_;
    std::vector<cl_int2> vertex_pos_;
};

}  // namespace null_engine::multithread::detail
