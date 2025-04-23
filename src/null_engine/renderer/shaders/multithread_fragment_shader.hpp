#pragma once

#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/acceleration/kernel.hpp>
#include <null_engine/acceleration/program.hpp>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/scene/lights/light_interface.hpp>

namespace null_engine::multithread::detail {

class NoopFragmentShader {
public:
    Program GetProgram() const;

    ArgsInfo GetArgs() const;
};

class MainFragmentShader {
public:
    static constexpr uint32_t kMaxNumberLights = 1;

    struct ShadowsMaps {
        compute::buffer depth_buffer;
        cl_int buffer_offsets[kMaxNumberLights];
    };

    explicit MainFragmentShader(AccelerationContext context);

    Program GetProgram() const;

    ArgsInfo GetArgs() const;

    void FillSceneInfo(
        Kernel::Args kernel_args, Vec3 view_pos, const std::vector<AnyLight>& lights, const ShadowsMaps& shadows_info
    ) const;

    void FillMaterialInfo(Kernel::Args kernel_args, const Material& material) const;

private:
    void FillTextureArgument(Kernel::Args kernel_args, size_t index, const std::optional<TextureView>& tex) const;

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
        LightDescription lights[kMaxNumberLights];
    };

    struct MaterialInfo {
        cl_int has_diffuse_tex;
        cl_int has_specular_tex;
        cl_int has_emission_tex;
        cl_float shininess;
    };

    compute::image2d empty_texture_;
};

Program GetFragmentShaderDefenitions();

}  // namespace null_engine::multithread::detail
