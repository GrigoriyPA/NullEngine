#pragma once

#include <CL/cl_platform.h>

#include <SFML/OpenGL.hpp>
#include <boost/compute/interop/opengl/opengl_texture.hpp>
#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/acceleration/buffer.hpp>
#include <null_engine/acceleration/kernel.hpp>
#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/renderer/shaders/multithread_fragment_shader.hpp>
#include <null_engine/renderer/shaders/vertex_shader.hpp>

namespace null_engine::multithread::detail {

struct RasterizerBuffer {
    compute::opengl_texture colors;
    compute::buffer depth;
};

class Rasterizer {
public:
    struct ViewInfo {
        uint64_t width;
        uint64_t height;
    };

private:
    using TriangleIndex = null_engine::detail::TriangleIndex;
    using InterpVertex = null_engine::detail::InterpVertex;

    struct SharedBuffers {
        compute::buffer work_batches;
        compute::buffer number_triangles;
    };

    class RasterizationKernel {
        enum KernelArgs {
            KA_VIEW_SIZE,
            KA_VIEW,
            KA_DEPTH,
            KA_WORK_SIZE,
            KA_WORK,
            KA_POINTS,
            KA_NUMBER_TRIANGLES,
            KA_WORK_OFFSET,
            KA_SHADER_PARAMS,
        };

    public:
        explicit RasterizationKernel(
            ViewInfo view, const SharedBuffers& buffers, const FragmentShader& fragment_shader,
            AccelerationContext context
        );

        static Program GetProgram(const FragmentShader& fragment_shader);

        Kernel::Args GetShaderArgs();

        void Run(const compute::buffer& vertices_info_buffer, const RasterizerBuffer& buffer);

    private:
        cl_int2 view_size_;
        Kernel kernel_;
    };

    class DistributionKernel {
        enum KernelArgs {
            KA_INDICES_SIZE,
            KA_INDICES,
            KA_WORK_SIZE,
            KA_WORK,
            KA_POINTS,
            KA_NUMBER_TRIANGLES,
        };

        static constexpr cl_int kLocalSize = 256;

    public:
        DistributionKernel(ViewInfo view, const SharedBuffers& buffers, AccelerationContext context);

        static Program GetProgram();

        void Run(const compute::buffer& vertices_info_buffer, const std::vector<TriangleIndex>& indices);

    private:
        DynamicBuffer<cl_int3> indices_;
        Kernel kernel_;
    };

    class CleanupKernel {
        enum KernelArgs {
            KA_WORK_SIZE,
            KA_NUMBER_TRIANGLES,
        };

        static constexpr cl_int2 kLocalSize = {.x = 16, .y = 16};

    public:
        CleanupKernel(ViewInfo view, const SharedBuffers& buffers, AccelerationContext context);

        static Program GetProgram();

        void Run();

    private:
        cl_int2 work_size_;
        Kernel kernel_;
    };

    static constexpr cl_int2 kWorkShape = {.x = 16, .y = 16};
    static constexpr uint32_t kTrianglesInBatch = 200;
    static constexpr uint32_t kNumberWorks = 10;

public:
    Rasterizer(ViewInfo view, const FragmentShader& fragment_shader, AccelerationContext context);

    void SetSceneInfo(const FragmentShader& shader, Vec3 view_pos, const std::vector<AnyLight>& lights);

    void SetMaterialInfo(const FragmentShader& shader, const Material& material);

    void DrawTriangles(
        const std::vector<InterpVertex>& points, const std::vector<TriangleIndex>& indices, RasterizerBuffer& buffer
    );

private:
    void FillVerticesInfoBuffer(const std::vector<InterpVertex>& points);

    SharedBuffers CreateBuffers(const ViewInfo& view);

    static cl_int2 GetWorkSize(const ViewInfo& view);

    static Program GetRasterizerDefenitions();

    struct VertexInfo {
        cl_float4 pos;
        cl_float3 color;
        cl_float3 normal;
        cl_float2 tex_coords;
        cl_float3 frag_pos;
    };

    struct WorkBatch {
        cl_int3 triangles[kTrianglesInBatch];
    };

    AccelerationContext context_;
    SharedBuffers shared_buffers_;
    DynamicBuffer<VertexInfo> vertices_info_;
    RasterizationKernel rasterization_kernel_;
    DistributionKernel distribution_kernel_;
    CleanupKernel cleanup_kernel_;
};

}  // namespace null_engine::multithread::detail
