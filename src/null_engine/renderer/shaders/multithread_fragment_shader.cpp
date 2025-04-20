#include "multithread_fragment_shader.hpp"

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <fmt/format.h>

#include <boost/compute/utility/source.hpp>
#include <null_engine/acceleration/helpers.hpp>
#include <null_engine/acceleration/program.hpp>
#include <null_engine/scene/lights/light.hpp>
#include <string>

namespace null_engine::multithread::detail {

FragmentShader::FragmentShader(AccelerationContext context)
    : empty_texture_(context.GetContext(), 1, 1, compute::image_format(CL_RGBA, CL_UNSIGNED_INT8)) {
}

Program FragmentShader::GetProgram() {
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
        .Define("MAX_NUMBER_LIGHTS", kMaxNumberLights)
        .Include(GetLightsProgram())
        .Build();
}

ArgsInfo FragmentShader::GetArgs() {
    return ArgsInfo()
        .AddArg("__read_only image2d_t", "diffuse_tex")
        .AddArg("__read_only image2d_t", "specular_tex")
        .AddArg("__read_only image2d_t", "emission_tex")
        .AddArg("SceneInfo", "scene", true)
        .AddArg("MaterialInfo", "material", true);
}

void FragmentShader::FillSceneInfo(Kernel::Args kernel_args, Vec3 view_pos, const std::vector<AnyLight>& lights) const {
    assert(lights.size() <= kMaxNumberLights && "Too many lights provided");

    SceneInfo scene = {
        .view_pos = Vec3ToCl(view_pos),
        .number_lights = static_cast<cl_int>(lights.size()),
    };
    for (uint32_t i = 0; const auto& light : lights) {
        scene.lights[i++] = light.GetDescription();
    }

    kernel_args.SetData(KA_SCENE, scene);
}

void FragmentShader::FillMaterialInfo(Kernel::Args kernel_args, const Material& material) const {
    FillTextureArgument(kernel_args, KA_DIFFUSE_TEX, material.diffuse_tex);
    FillTextureArgument(kernel_args, KA_SPECULAR_TEX, material.specular_tex);
    FillTextureArgument(kernel_args, KA_EMISSION_TEX, material.emission_tex);

    kernel_args.SetData<MaterialInfo>(
        KA_MATERIAL,
        {
            .has_diffuse_tex = !!material.diffuse_tex,
            .has_specular_tex = !!material.specular_tex,
            .has_emission_tex = !!material.emission_tex,
            .shininess = material.shininess,
        }
    );
}

void FragmentShader::FillTextureArgument(Kernel::Args kernel_args, size_t index, const std::optional<TextureView>& tex)
    const {
    kernel_args.SetVal(index, tex ? tex->GetDeviceBuffer() : empty_texture_);
}

}  // namespace null_engine::multithread::detail
