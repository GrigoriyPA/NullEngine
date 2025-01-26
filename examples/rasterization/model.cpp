#include "model.hpp"

#include <iostream>
#include <null_engine/drawable_objects/primitive_objects.hpp>
#include <null_engine/scene/animations/primitive_animations.hpp>
#include <null_engine/scene/lights/light.hpp>
#include <null_engine/util/interface/helpers/constants.hpp>
#include <numbers>

namespace null_engine::tests {

namespace {

constexpr const char* kDiffuseTexturePath = "../../assets/textures/box_diffuse.png";
constexpr const char* kSpecularTexturePath = "../../assets/textures/box_specular.png";
constexpr const char* kEmissionTexturePath = "../../assets/textures/box_emission.jpg";

constexpr LightStrength kLightStrength = {.ambient = 0.2, .diffuse = 0.6, .specular = 0.8};
constexpr AttenuationSettings kLightAttenuation = {.constant = 1.0, .quadratic = 0.1};
const CameraOrientation kCameraPos = {
    .position = Vec3(0.0, 0.0, 0.0), .direction = Vec3(0.0, 0.0, 1.0), .horizon = Vec3(1.0, 0.0, 0.0)
};

ModelAssetes LoadAssets() {
    ModelAssetes assets;
    assets.textures.emplace_back(Texture::LoadFromFile(kDiffuseTexturePath));
    assets.textures.emplace_back(Texture::LoadFromFile(kSpecularTexturePath));
    assets.textures.emplace_back(Texture::LoadFromFile(kEmissionTexturePath));

    return assets;
}

void AddAmbientLight(Scene& scene) {
    const auto strength = 0.6;
    scene.AddLight(AmbientLight(strength));
}

void AddDirectLight(Scene& scene) {
    const Vec3 light_direction(2.0, -1.0, 3.0);
    const DirectLight light(light_direction, kLightStrength);

    scene.AddLight(light);

    const Vec3 visualization_pos = Vec3(0.0, 0.0, 2.0) - light_direction * 0.5;
    const auto visualization_scale = 0.2;
    scene.EmplaceObject(light.VisualizeLight(visualization_pos, kWhite, visualization_scale));
}

void AddPointLight(Scene& scene) {
    const Vec3 light_position(-1.0, 1.0, 1.0);
    const PointLight light(light_position, kLightStrength, kLightAttenuation);

    scene.AddLight(light);

    const auto visualization_scale = 0.2;
    scene.EmplaceObject(light.VisualizeLight(kWhite, visualization_scale));
}

void AddSpotLight(Scene& scene) {
    const SpotLight light(
        {
            .position = Vec3(-1.0, 1.0, 1.0),
            .direction = Vec3(1.0, -1.0, 1.0),
            .light_angle = std::numbers::pi / 6.0,
            .light_angle_ratio = 1.2,
        },
        kLightStrength, kLightAttenuation
    );

    scene.AddLight(light);

    const auto visualization_scale = 0.2;
    scene.EmplaceObject(light.VisualizeLight(kWhite, visualization_scale));
}

void AddCameraLight(const CameraBase& camera, Scene& scene) {
    auto camera_light = SceneLight::Make(SpotLight(
        {
            .light_angle = std::numbers::pi / 6.0,
            .light_angle_ratio = 1.2,
        },
        kLightStrength, kLightAttenuation
    ));
    camera.SubscribeOnCameraTransform(camera_light->GetTransformPort());

    scene.AddLight(std::move(camera_light));
}

void SetRotationAnimation(AnimatorRegistry& animator_registry, SceneObject& object) {
    const auto rotation_axis = Vec3(1.0, 1.0, 1.0);
    const auto rotation_speed = std::numbers::pi / 3.0;
    auto animator = std::make_unique<RotationAnimation>(rotation_axis, rotation_speed);
    animator->SubscribeOnAnimation(object.GetTransformPort());
    animator_registry.AddAnimator(std::move(animator));
}

void SetTranslationAnimation(AnimatorRegistry& animator_registry, SceneObject& object) {
    const Vec3 start_pos(0.0, 0.0, 0.0);
    const Vec3 end_pos(0.0, 0.0, 3.0);
    const auto speed = (start_pos - end_pos).norm() / 5.0;
    auto animator = std::make_unique<TranslationAnimation>(start_pos, end_pos, speed);
    animator->SubscribeOnAnimation(object.GetTransformPort());
    animator_registry.AddAnimator(std::move(animator));
}

void AddQuad(AnimatorRegistry& animator_registry, const ModelAssetes& assets, Scene& scene) {
    const auto quad_instance = Translation(0.0, 0.0, 2.0);
    SceneObject quad(
        CreateQuad()
            .SetMaterial({
                // .diffuse_tex = TextureView(*assets.textures[0]),
                // .specular_tex = TextureView(*assets.textures[1]),
                .shininess = 20.0,
            })
            .SetColors({kGreen, kBlue, kRed, kWhite}),
        quad_instance
    );
    // SetRotationAnimation(animator_registry, quad);

    scene.AddObject(std::move(quad));
}

void AddCube(AnimatorRegistry& animator_registry, const ModelAssetes& assets, Scene& scene) {
    const auto cube_instance = Translation(0.0, 0.0, 2.0);
    SceneObject cube(
        CreateCube()
            .SetMaterial({
                .diffuse_tex = TextureView(*assets.textures[0]),
                .specular_tex = TextureView(*assets.textures[1]),
                .shininess = 20.0,
            })
            .SetColors(kWhite * 0.8),
        cube_instance
    );
    // SetRotationAnimation(animator_registry, cube);

    scene.AddObject(std::move(cube));
}

Scene CreateScene(AnimatorRegistry& animator_registry, const ModelAssetes& assets, const CameraBase& camera) {
    Scene scene;
    AddQuad(animator_registry, assets, scene);
    // AddCube(animator_registry, assets, scene);

    // AddCameraLight(std::move(camera_light));
    // AddAmbientLight(AmbientLight(0.6));
    // AddDirectLight(scene);
    // AddPointLight(scene);
    // AddSpotLight(scene);

    return scene;
}

PerspectiveCamera CreatePerspectiveCamera(uint64_t view_width, uint64_t view_height) {
    return PerspectiveCamera(
        kCameraPos,
        {
            .fov = std::numbers::pi / 2.0,
            .ratio = static_cast<FloatType>(view_width) / static_cast<FloatType>(view_height),
            .min_distance = 0.1,
            .max_distance = 250.0,
        }
    );
}

DirectCamera CreateDirectCamera() {
    return DirectCamera(
        kCameraPos,
        {
            .width = 2.0,
            .height = 2.0,
            .depth = 2.0,
        }
    );
}

}  // anonymous namespace

Model::Model(uint64_t view_width, uint64_t view_height, bool multithread_rendering)
    : assets_(LoadAssets())
    , acceleration_context_(AccelerationContext::Create())
    , camera_(CreatePerspectiveCamera(view_width, view_height))
    , scene_(CreateScene(animator_registry_, assets_, camera_))
    , native_renderer_({view_width, view_height})
    , multithread_renderer_({view_width, view_height}, acceleration_context_)
    , multithread_rendering_(multithread_rendering)
    , in_texture_port_(std::bind(&Model::OnNativeRenderedTexture, this, std::placeholders::_1))
    , in_texture_id_port_(std::bind(&Model::OnMultithreadRenderedTexture, this, std::placeholders::_1)) {
    native_renderer_.SubscribeToTextures(&in_texture_port_);
    multithread_renderer_.SubscribeToTextures(&in_texture_id_port_);

    std::cout << "Discovered device:\n" << acceleration_context_.GetDeviceDescription() << "\n";
}

void Model::SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const {
    DrawViewEvent event = {.delta_time = current_delta_time_};
    if (multithread_rendering_) {
        event.render_texture = current_texture_id_;
    } else {
        event.render_texture = current_texture_;
    }

    out_draw_event_port_->Subscribe(observer_port, event);
}

void Model::DoRendering() {
    if (multithread_rendering_) {
        multithread_renderer_.GetRenderPort()->OnEvent({.scene = scene_, .camera = camera_});
    } else {
        native_renderer_.GetRenderPort()->OnEvent({.scene = scene_, .camera = camera_});
    }
}

void Model::MoveCamera(const CameraChange& camera_change) {
    camera_.GetChangePort()->OnEvent(camera_change);
}

void Model::Refresh(FloatType delta_time) {
    current_delta_time_ = delta_time;
    animator_registry_.GetRefreshPort()->OnEvent(delta_time);
}

void Model::OnNativeRenderedTexture(const TextureData& texture) {
    current_texture_ = texture;
    out_draw_event_port_->Notify({.delta_time = current_delta_time_, .render_texture = current_texture_});
}

void Model::OnMultithreadRenderedTexture(GLuint texture_id) {
    current_texture_id_ = texture_id;
    out_draw_event_port_->Notify({.delta_time = current_delta_time_, .render_texture = current_texture_id_});
}

}  // namespace null_engine::tests
