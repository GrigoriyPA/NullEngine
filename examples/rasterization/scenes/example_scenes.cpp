#include "example_scenes.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <null_engine/drawable_objects/material/texture.hpp>
#include <null_engine/drawable_objects/primitive_objects.hpp>
#include <null_engine/renderer/camera/camera.hpp>
#include <null_engine/scene/animations/primitive_animations.hpp>
#include <null_engine/scene/lights/light.hpp>
#include <null_engine/util/geometry/matrix.hpp>
#include <null_engine/util/geometry/vector.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>
#include <numbers>

namespace null_engine::tests {

namespace {

constexpr char kAssetesPath[] = "../../../assets";

constexpr char kTexturesPath[] = "textures";
constexpr char kDiffuseTexturePath[] = "box_diffuse.png";
constexpr char kSpecularTexturePath[] = "box_specular.png";
constexpr char kEmissionTexturePath[] = "box_emission.jpg";

constexpr char kObjectsPath[] = "3d_objects";
constexpr const char* kCubePath = "cube.obj";
constexpr const char* kMjolnirPath = "mjolnir.glb";
constexpr const char* kVelorumPath = "system_velorum.glb";

PerspectiveCamera CreatePerspectiveCamera(CameraOrientation orientation, uint64_t view_width, uint64_t view_height) {
    return PerspectiveCamera(
        orientation,
        {
            .fov = std::numbers::pi * 0.3,
            .ratio = static_cast<FloatType>(view_width) / static_cast<FloatType>(view_height),
            .min_distance = 0.1,
            .max_distance = 250.0,
        }
    );
}

DirectCamera CreateDirectCamera(CameraOrientation orientation) {
    return DirectCamera(
        orientation,
        {
            .width = 2.0,
            .height = 2.0,
            .depth = 2.0,
        }
    );
}

}  // anonymous namespace

SceneInfo::SceneInfo(const Settings& settings, CameraOrientation camera_orientation)
    : acceleration_context_(settings.acceleration_context)
    , object_loader_({.verbose = true, .acceleration_context = acceleration_context_})
    , textures_(TEX_MAX)
    , camera_(CreatePerspectiveCamera(camera_orientation, settings.view_width, settings.view_height)) {
    SetupTextures();
}

SceneInfo::Ptr SceneInfo::SimpleQuad(const Settings& settings) {
    auto result = std::make_unique<SceneInfo>(
        settings,
        CameraOrientation{
            .position = Vec3(0.0, 0.0, -2.0), .direction = Vec3(0.0, 0.0, 1.0), .horizon = Vec3(1.0, 0.0, 0.0)
        }
    );

    const auto quad_instance = Translation(0.0, 0.0, 2.0);
    SceneObject quad(
        CreateQuad(false)
            .SetMaterial({
                .diffuse_tex = result->GetTexture(TEX_DIFFUSE),
                .shininess = 20.0,
            })
            .SetColors({kGreen, kBlue, kRed, kWhite}),
        quad_instance
    );

    result->SetRotationAnimation(quad);
    result->GetScene().AddObject(std::move(quad));

    return result;
}

SceneInfo::Ptr SceneInfo::SimpleCubes(const Settings& settings) {
    auto result = std::make_unique<SceneInfo>(
        settings,
        CameraOrientation{
            .position = Vec3(0.0, 0.0, -2.0), .direction = Vec3(0.0, 0.0, 1.0), .horizon = Vec3(1.0, 0.0, 0.0)
        }
    );

    const auto cube_instance = Translation(1.0, 1.0, 3.0);
    SceneObject cube(
        CreateCube()
            .SetMaterial({
                .diffuse_tex = result->GetTexture(TEX_DIFFUSE),
                .specular_tex = result->GetTexture(TEX_SPECULAR),
                // .emission_tex = result->GetTexture(TEX_EMISSION),
                .shininess = 20.0,
            })
            .SetColors(kWhite * 0.8),
        cube_instance
    );
    cube.AddInstance(Translation(0.0, 0.0, 2.0));

    const Vec3 rotation_axis(1.0, 1.0, 1.0);
    result->SetRotationAnimation(cube, rotation_axis);
    // SetTranslationAnimation(animator_registry, cube);

    result->GetScene().AddObject(std::move(cube));
    result->AddDirectLight();

    return result;
}

SceneInfo::Ptr SceneInfo::LoadCube(const Settings& settings) {
    auto result = std::make_unique<SceneInfo>(
        settings,
        CameraOrientation{
            .position = Vec3(0.0, 0.0, -2.0), .direction = Vec3(0.0, 0.0, 1.0), .horizon = Vec3(1.0, 0.0, 0.0)
        }
    );

    const auto object_instance = Translation(0.0, 0.0, 2.0);
    result->GetScene().AddObject(result->LoadObject(kCubePath, object_instance));
    result->AddDirectLight();

    return result;
}

SceneInfo::Ptr SceneInfo::LoadMjolnir(const Settings& settings, LightType light_type) {
    auto result = std::make_unique<SceneInfo>(settings);

    const auto plane_instansce =
        Translation(0.0, -4.0, 0.0) * Rotation(Vec3(1.0, 0.0, 0.0), std::numbers::pi / 2.0) * Scale(14.0);
    SceneObject plane(
        CreateQuad(true)
            .SetMaterial({
                .specular_tex = result->GetTexture(TEX_MONOTONIC),
                .shininess = 20.0,
            })
            .SetColors(Vec4(0.6, 0.6, 0.6, 1.0)),
        plane_instansce
    );
    result->GetScene().AddObject(std::move(plane));

    SceneObject object(Ident());
    const auto object_instance = Translation(0.0, 0.0, 2.0);
    object.AddChild(result->LoadObject(kMjolnirPath, object_instance));
    result->SetRotationAnimation(object);
    result->GetScene().AddObject(std::move(object));

    switch (light_type) {
        case LightType::Direct: {
            const Vec3 direction(1.0, -4.0, 1.0);
            result->AddDirectLight(
                direction, kDefaultLightStrength,
                DirectLight::ShadowSettings{
                    .position = -direction * 0.5,
                    .size = Vec3(10.0, 10.0, 10.0),
                    .resolution = 0.01,
                }
            );
            break;
        }
        case LightType::Point: {
            result->AddPointLight();
            break;
        }
        case LightType::Spot: {
            const Vec3 position(-4.0, 2.0, -4.0);
            const Vec3 direction(1.0, -1.7, 1.0);
            result->AddSpotLight(
                position, direction, kDefaultLightStrength, {.constant = 1.0, .quadratic = 0.01},
                SpotLight::ShadowSettings{
                    .min_distance = 1,
                    .max_distance = 15.0,
                    .resolution = 0.03,
                }
            );
            break;
        }
    }

    return result;
}

SceneInfo::Ptr SceneInfo::LoadVelorum(const Settings& settings) {
    const Vec3 direction(1.0, -1.0, -1.0);
    auto result = std::make_unique<SceneInfo>(
        settings,
        CameraOrientation{.position = Vec3(-40.0, 40.0, 40.0), .direction = direction, .horizon = Horizon(direction)}
    );

    const auto object_instance = Scale(0.05);
    result->GetScene().AddObject(result->LoadObject(kVelorumPath, object_instance));
    result->AddDirectLight(direction);

    return result;
}

void SceneInfo::OnRefreshEvent(FloatType delta_time) {
    animator_registry_.GetRefreshPort()->OnEvent(delta_time);
}

void SceneInfo::OnCameraEvent(const CameraChange& camera_change) {
    camera_.GetChangePort()->OnEvent(camera_change);
}

Scene& SceneInfo::GetScene() {
    return scene_;
}

AnyCameraRef SceneInfo::GetCamera() {
    return camera_;
}

TextureView SceneInfo::GetTexture(size_t index) {
    assert(index < textures_.size() && "Texture index too large");
    return TextureView(*textures_[index]);
}

SceneObject SceneInfo::LoadObject(const std::string& path, Transform instance) {
    auto uploaded_object = object_loader_.LoadFromFile(std::filesystem::path(kAssetesPath) / kObjectsPath / path);
    uploaded_object.GetTransformPort()->OnEvent(instance);

    const auto& statistic = uploaded_object.GetStatistic();
    std::cout << "Loaded object '" << path << "', info:\n"
              << "- Points: " << statistic.number_points << "\n- Faces: " << statistic.number_faces
              << "\n- Sub objects: " << statistic.number_objects << "\n- Depth: " << statistic.max_depth << "\n";

    return uploaded_object;
}

void SceneInfo::SetupTextures() {
    const auto base_path = std::filesystem::path(kAssetesPath) / kTexturesPath;
    textures_[TEX_DIFFUSE] = Texture::LoadFromFile(base_path / kDiffuseTexturePath);
    textures_[TEX_SPECULAR] = Texture::LoadFromFile(base_path / kSpecularTexturePath);
    textures_[TEX_EMISSION] = Texture::LoadFromFile(base_path / kEmissionTexturePath);
    textures_[TEX_MONOTONIC] = Texture::Monotonic(kWhite);

    if (acceleration_context_) {
        for (auto& texture : textures_) {
            texture->ToDevice(*acceleration_context_);
        }
    }
}

SceneInfo& SceneInfo::AddAmbientLight(FloatType strength) {
    scene_.AddLight(AmbientLight(strength));
    return *this;
}

SceneInfo& SceneInfo::AddDirectLight(
    Vec3 direction, LightStrength strength, std::optional<DirectLight::ShadowSettings> shadow
) {
    DirectLight light(direction, strength);
    if (shadow) {
        light.SetupShadow(*shadow);
        // scene_.EmplaceObject(light.VisualizeShadowBox());
    }

    scene_.AddLight(light);

    if (!acceleration_context_) {
        const Vec3 visualization_pos = Vec3(0.0, 0.0, 2.0) - direction * 0.5;
        const auto visualization_scale = 0.2;
        scene_.EmplaceObject(light.VisualizeLight(visualization_pos, kWhite, visualization_scale));
    }
    return *this;
}

SceneInfo& SceneInfo::AddPointLight(Vec3 position, LightStrength strength, AttenuationSettings attenuation) {
    const PointLight light(position, strength, attenuation);
    scene_.AddLight(light);

    if (!acceleration_context_) {
        const auto visualization_scale = 0.2;
        scene_.EmplaceObject(light.VisualizeLight(kWhite, visualization_scale));
    }
    return *this;
};

SceneInfo& SceneInfo::AddSpotLight(
    Vec3 position, Vec3 direction, LightStrength strength, AttenuationSettings attenuation,
    std::optional<SpotLight::ShadowSettings> shadow
) {
    SpotLight light(
        {.position = position, .direction = direction, .light_angle = std::numbers::pi / 3.0, .light_angle_ratio = 1.2},
        strength, attenuation
    );
    if (shadow) {
        light.SetupShadow(*shadow);
        // scene_.EmplaceObject(light.VisualizeShadowBox());
    }

    scene_.AddLight(light);

    if (!acceleration_context_) {
        const auto visualization_scale = 0.2;
        scene_.EmplaceObject(light.VisualizeLight(kWhite, visualization_scale));
    }
    return *this;
}

SceneInfo& SceneInfo::SetRotationAnimation(SceneObject& object, Vec3 axis, FloatType speed) {
    auto animator = std::make_unique<RotationAnimation>(axis, speed);
    animator->SubscribeOnAnimation(object.GetTransformPort());
    animator_registry_.AddAnimator(std::move(animator));
    return *this;
}

SceneInfo& SceneInfo::SetTranslationAnimation(SceneObject& object, Vec3 start, Vec3 end, FloatType speed) {
    auto animator = std::make_unique<TranslationAnimation>(start, end, speed);
    animator->SubscribeOnAnimation(object.GetTransformPort());
    animator_registry_.AddAnimator(std::move(animator));
    return *this;
}

}  // namespace null_engine::tests
