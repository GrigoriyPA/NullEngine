#pragma once

#include <memory>
#include <null_engine/renderer/camera/camera.hpp>
#include <null_engine/renderer/camera/camera_interface.hpp>
#include <null_engine/scene/animations/animator.hpp>
#include <null_engine/scene/lights/light.hpp>
#include <null_engine/scene/objects/object_loader.hpp>
#include <null_engine/scene/scene.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/geometry/matrix.hpp>
#include <null_engine/util/geometry/vector.hpp>
#include <numbers>

namespace null_engine::tests {

class SceneInfo {
    using AccelerationContext = multithread::AccelerationContext;

    static constexpr LightStrength kDefaultLightStrength = {.ambient = 0.2, .diffuse = 0.6, .specular = 0.8};
    static constexpr AttenuationSettings kDefaultLightAttenuation = {.constant = 1.0, .quadratic = 0.05};

public:
    using Ptr = std::unique_ptr<SceneInfo>;

    enum Textures {
        TEX_DIFFUSE,
        TEX_SPECULAR,
        TEX_EMISSION,
        TEX_MONOTONIC,
        TEX_MAX,
    };

    enum class LightType {
        Direct,
        Point,
        Spot,
    };

    struct Settings {
        uint64_t view_width;
        uint64_t view_height;
        std::optional<AccelerationContext> acceleration_context;
    };

    explicit SceneInfo(
        const Settings& settings,
        CameraOrientation camera_orientation =
            {.position = Vec3(0.0, 0.0, -3.0), .direction = Vec3(0.0, -1.0, 1.0), .horizon = Vec3(1.0, 0.0, 0.0)}
    );

    static SceneInfo::Ptr SimpleQuad(const Settings& settings);

    static SceneInfo::Ptr SimpleCubes(const Settings& settings);

    static SceneInfo::Ptr LoadCube(const Settings& settings);

    static SceneInfo::Ptr LoadMjolnir(const Settings& settings, LightType light_type = LightType::Spot);

    static SceneInfo::Ptr LoadVelorum(const Settings& settings);

    void OnRefreshEvent(FloatType delta_time);

    void OnCameraEvent(const CameraChange& camera_change);

    Scene& GetScene();

    AnyCameraRef GetCamera();

    TextureView GetTexture(size_t index);

    SceneObject LoadObject(const std::string& path, Transform instance);

    SceneInfo& AddAmbientLight(FloatType strength = 0.6);

    SceneInfo& AddDirectLight(
        Vec3 direction = Vec3(2.0, -1.0, 3.0), LightStrength strength = kDefaultLightStrength,
        std::optional<DirectLight::ShadowSettings> shadow = std::nullopt
    );

    SceneInfo& AddPointLight(
        Vec3 position = Vec3(-2.0, 0.0, -2.0), LightStrength strength = kDefaultLightStrength,
        AttenuationSettings attenuation = kDefaultLightAttenuation
    );

    SceneInfo& AddSpotLight(
        Vec3 position = Vec3(-2.0, 0.0, -2.0), Vec3 direction = Vec3(1.0, -1.0, 1.0),
        LightStrength strength = kDefaultLightStrength, AttenuationSettings attenuation = kDefaultLightAttenuation,
        std::optional<SpotLight::ShadowSettings> shadow = std::nullopt
    );

    SceneInfo& SetRotationAnimation(
        SceneObject& object, Vec3 axis = Vec3(0.0, 1.0, 0.0), FloatType speed = std::numbers::pi / 5.0
    );

    SceneInfo& SetTranslationAnimation(
        SceneObject& object, Vec3 start = Vec3(0.0, 0.0, 0.0), Vec3 end = Vec3(0.0, 0.0, 3.0), FloatType speed = 0.2
    );

private:
    void SetupTextures();

    std::optional<AccelerationContext> acceleration_context_;
    ObjectLoader object_loader_;
    AnimatorRegistry animator_registry_;
    std::vector<Texture::Ptr> textures_;
    Scene scene_;
    PerspectiveCamera camera_;
};

}  // namespace null_engine::tests
