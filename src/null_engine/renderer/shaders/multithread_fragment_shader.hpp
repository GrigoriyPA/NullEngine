#pragma once

#include <null_engine/acceleration/acceleration_context.hpp>
#include <null_engine/acceleration/kernel_program.hpp>
#include <null_engine/drawable_objects/material/material.hpp>
#include <null_engine/scene/lights/light_interface.hpp>

namespace null_engine::multithread::detail {

class FragmentShader {
public:
    static constexpr uint32_t kMaxNumberLights = 1;

    explicit FragmentShader(AccelerationContext context);

    static Program GetKernelProgram();

    static std::string GetArguments();

    static std::string GetShaderCall(const std::string& vertex_variable);

    void FillSceneInfo(
        compute::kernel& kernel, uint32_t argument_offset, Vec3 view_pos, const std::vector<AnyLight>& lights
    ) const;

    void FillMaterialInfo(compute::kernel& kernel, uint32_t argument_offset, const Material& material) const;

private:
    void FillTextureArgument(compute::kernel& kernel, uint32_t argument_offset, const std::optional<TextureView>& tex)
        const;

    compute::context context_;
    compute::image2d empty_texture_;
};

}  // namespace null_engine::multithread::detail
