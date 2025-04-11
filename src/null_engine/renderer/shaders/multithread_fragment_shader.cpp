#include "multithread_fragment_shader.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>

#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>

namespace null_engine::multithread::detail {

namespace {

enum KernelArgs {
    KA_DIFFUSE_TEX,
    KA_SPECULAR_TEX,
    KA_EMISSION_TEX,
    KA_SCENE,
    KA_MATERIAL,
};

struct SceneInfo {
    cl_float3 view_pos;
};

struct MaterialInfo {
    cl_int has_diffuse_tex;
    cl_int has_specular_tex;
    cl_int has_emission_tex;
    cl_float shininess;
};

const std::string kCalculateFragmentColorSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
    typedef struct { float3 view_pos; } SceneInfo;

    typedef struct {
        int has_diffuse_tex;
        int has_specular_tex;
        int has_emission_tex;
        float shininess;
    } MaterialInfo;

    typedef struct {
        float3 color;
        float3 normal;
        float2 tex_coords;
        float3 frag_pos;
    } InterpolationParams;

    float3 CalculateFragmentColor(
        __read_only image2d_t diffuse_tex, __read_only image2d_t specular_tex, __read_only image2d_t emission_tex,
        const SceneInfo* scene, const MaterialInfo* material, const InterpolationParams* params
    ) {
        float3 diffuse_color = params->color;
        const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
        if (material->has_diffuse_tex) {
            diffuse_color = read_imagef(diffuse_tex, sampler, params->tex_coords).xyz;
        }

        float3 result_color = (float3)(0.0f, 0.0f, 0.0f);
        if (material->has_emission_tex) {
            result_color = read_imagef(emission_tex, sampler, params->tex_coords).xyz;
        }

        return diffuse_color + result_color;
    };
);

}  // anonymous namespace

FragmentShader::FragmentShader(AccelerationContext context)
    : context_(context.GetContext())
    , empty_texture_(context_, 1, 1, compute::image_format(CL_RGBA, CL_UNSIGNED_INT8)) {
}

std::string FragmentShader::GetSource() {
    return kCalculateFragmentColorSource;
}

std::string FragmentShader::GetArguments() {
    return "__read_only image2d_t diffuse_tex, __read_only image2d_t specular_tex, __read_only image2d_t emission_tex, "
           "SceneInfo scene, MaterialInfo material";
}

std::string FragmentShader::GetShaderCall(const std::string& vertex_variable, const std::string& output_varianle) {
    std::stringstream shader_call;

    shader_call << "const float3 " << output_varianle
                << " = CalculateFragmentColor(diffuse_tex, specular_tex, emission_tex, &scene, &material, &"
                << vertex_variable << ");";

    return shader_call.str();
}

void FragmentShader::FillSceneInfo(compute::kernel& kernel, uint32_t argument_offset, Vec3 view_pos) const {
    SceneInfo cl_scene = {.view_pos = Vec3ToCl(view_pos)};
    kernel.set_arg(argument_offset + KA_SCENE, sizeof(SceneInfo), &cl_scene);
}

void FragmentShader::FillMaterialInfo(compute::kernel& kernel, uint32_t argument_offset, const Material& material)
    const {
    FillTextureArgument(kernel, argument_offset + KA_DIFFUSE_TEX, material.diffuse_tex);
    FillTextureArgument(kernel, argument_offset + KA_SPECULAR_TEX, material.specular_tex);
    FillTextureArgument(kernel, argument_offset + KA_EMISSION_TEX, material.emission_tex);

    MaterialInfo cl_material = {
        .has_diffuse_tex = !!material.diffuse_tex,
        .has_specular_tex = !!material.specular_tex,
        .has_emission_tex = !!material.emission_tex,
        .shininess = material.shininess
    };
    kernel.set_arg(argument_offset + KA_MATERIAL, sizeof(MaterialInfo), &cl_material);
}

void FragmentShader::FillTextureArgument(
    compute::kernel& kernel, uint32_t argument_offset, const std::optional<TextureView>& tex
) const {
    kernel.set_arg(argument_offset, tex ? tex->GetDeviceBuffer() : empty_texture_);
}

}  // namespace null_engine::multithread::detail
