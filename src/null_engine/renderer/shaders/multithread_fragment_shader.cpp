#include "multithread_fragment_shader.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <fmt/format.h>

#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/acceleration/kernel_program.hpp>
#include <null_engine/scene/lights/light.hpp>
#include <string>

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
    cl_int number_lights;
    LightDescription lights[FragmentShader::kMaxNumberLights];
};

struct MaterialInfo {
    cl_int has_diffuse_tex;
    cl_int has_specular_tex;
    cl_int has_emission_tex;
    cl_float shininess;
};

}  // anonymous namespace

FragmentShader::FragmentShader(AccelerationContext context)
    : context_(context.GetContext())
    , empty_texture_(context_, 1, 1, compute::image_format(CL_RGBA, CL_UNSIGNED_INT8)) {
}

Program FragmentShader::GetKernelProgram() {
    static constexpr std::string_view kFragmentShaderSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
        typedef struct {
            float3 view_pos;
            int number_lights;
            LightDescription lights[MAX_NUMBER_LIGHTS];
        } SceneInfo;

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

            if (scene->number_lights <= 0 || IsZeroFloat3(params->normal)) {
                return diffuse_color + result_color;
            }

            LightingMaterialSettings light_settings = {
                .frag_pos = params->frag_pos,
                .view_direction = normalize(scene->view_pos - params->frag_pos),
                .normal = normalize(params->normal),
                .diffuse_color = diffuse_color,
            };

            if (material->has_specular_tex) {
                light_settings.specular_color = read_imagef(specular_tex, sampler, params->tex_coords).xyz;
                light_settings.shininess = material->shininess;
            }

            for (int i = 0; i < MAX_NUMBER_LIGHTS; ++i) {
                if (i == scene->number_lights) {
                    break;
                }
                result_color += CalculateLighting(&scene->lights[i], &light_settings);
            }

            return result_color;
        }
    );

    return ProgramBuilder("FragmentShader", kFragmentShaderSource)
        .Define("MAX_NUMBER_LIGHTS", std::to_string(kMaxNumberLights))
        .Include(GetLightsProgram())
        .Build();
}

std::string FragmentShader::GetArguments() {
    return "__read_only image2d_t diffuse_tex, "
           "__read_only image2d_t specular_tex, "
           "__read_only image2d_t emission_tex, "
           "SceneInfo scene, "
           "MaterialInfo material";
}

std::string FragmentShader::GetShaderCall(const std::string& vertex_variable) {
    return fmt::format(
        "CalculateFragmentColor(diffuse_tex, specular_tex, emission_tex, &scene, &material, &{})", vertex_variable
    );
}

void FragmentShader::FillSceneInfo(
    compute::kernel& kernel, uint32_t argument_offset, Vec3 view_pos, const std::vector<AnyLight>& lights
) const {
    assert(lights.size() <= kMaxNumberLights && "Too many lights provided");

    SceneInfo cl_scene = {
        .view_pos = Vec3ToCl(view_pos),
        .number_lights = static_cast<cl_int>(lights.size()),
    };
    for (uint32_t i = 0; const auto& light : lights) {
        cl_scene.lights[i++] = light.GetDescription();
    }

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
