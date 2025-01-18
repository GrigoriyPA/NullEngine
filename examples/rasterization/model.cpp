#include "model.hpp"

#include <null_engine/drawable_objects/primitive_objects.hpp>
#include <null_engine/renderer/camera/camera.hpp>
#include <null_engine/scene/animations/primitive_animations.hpp>
#include <null_engine/tests/tests_constants.hpp>
#include <null_engine/tests/tests_helpers.hpp>
#include <numbers>

namespace null_engine::tests {

namespace {

constexpr const char* kTexturePath = "../../assets/textures/box_diffuse.png";

ModelAssetes LoadAssets() {
    return {.texture = Texture::LoadFromFile(kTexturePath)};
}

Scene CreateScene(AnimatorRegistry& animator_registry, const ModelAssetes& assets) {
    Scene scene;

    const std::vector<Vec3> sample_points = {Vec3(-0.5, -0.5, 0.5), Vec3(0, 0.5, 0.5),   Vec3(0.5, -0.7, 0.5),
                                             Vec3(0, 0.5, 0.5),     Vec3(0.5, 0.5, 1.5), Vec3(0.5, -0.7, 0.5)};
    const std::vector<VertexParams> sample_params = {VertexParams{.color = kRed},   VertexParams{.color = kBlue},
                                                     VertexParams{.color = kGreen}, VertexParams{.color = kBlue},
                                                     VertexParams{.color = kWhite}, VertexParams{.color = kGreen}};

    const Vec3 sample_translation(0.0, 0.0, 2.0);
    SceneObject traingles_object(Mat4::Translation(sample_translation));
    traingles_object.EmplaceChild(VerticesObject(sample_points.size(), VerticesObject::Type::Triangles)
                                      .SetPositions(sample_points)
                                      .SetParams(sample_params)
                                      .GenerateNormals(false));

    const auto normals_scale = 0.5;
    traingles_object.EmplaceChild(
        CreateNormalsVisualization(traingles_object.GetChild(0).GetObject(), kRed, normals_scale)
    );

    const auto rotation_axis = Vec3::Ident(1.0);
    const auto rotation_speed = std::numbers::pi / 2.0;
    auto animator = std::make_unique<RotationAnimation>(rotation_axis, rotation_speed);

    auto animation = Animation::Make();
    animator->SubscribeOnAnimation(animation->GetTransformPort());
    animator_registry.AddAnimator(std::move(animator));
    traingles_object.SetAnimation(std::move(animation));

    scene.AddObject(std::move(traingles_object));

    const std::vector<Vec3> triangle_points = {
        Vec3(-0.5, 0.0, 0.0),
        Vec3(0, 0.5, 0.0),
        Vec3(0.5, 0.0, 0.0),
    };
    const std::vector<VertexParams> triangle_params = {
        VertexParams{.tex_coords = Vec2(0.0, 1.0)},
        VertexParams{.tex_coords = Vec2(0.5, 0.5)},
        VertexParams{.tex_coords = Vec2(1.0, 1.0)},
    };
    const Vec3 triangle_translation(2.0, 0.0, 2.0);
    scene.EmplaceObject(
        VerticesObject(triangle_points.size(), VerticesObject::Type::Triangles)
            .SetPositions(triangle_points)
            .SetParams(triangle_params)
            .SetMaterial({.diffuse_tex = TextureView(*assets.texture)})
            .GenerateNormals(false),
        Mat4::Translation(triangle_translation)
    );

    return scene;
}

AnyMovableCamera CreateCamera(uint64_t view_width, uint64_t view_height) {
    const FloatType fov = std::numbers::pi / 2.0;
    const FloatType min_distance = 0.1;
    const FloatType max_distance = 250;
    return PerspectiveCamera(
        fov, static_cast<FloatType>(view_width) / static_cast<FloatType>(view_height), min_distance, max_distance
    );
}

}  // anonymous namespace

Model::Model(uint64_t view_width, uint64_t view_height)
    : assets_(LoadAssets())
    , scene_(CreateScene(animator_registry_, assets_))
    , camera_(CreateCamera(view_width, view_height))
    , renderer_({view_width, view_height})
    , in_texture_port_(std::bind(&Model::OnRenderedTexture, this, std::placeholders::_1)) {
    renderer_.SubscribeToTextures(&in_texture_port_);
}

void Model::SubscribeToDrawEvents(InPort<DrawViewEvent>* observer_port) const {
    out_draw_event_port_->Subscribe(
        observer_port, {.delta_time = current_delta_time_, .render_texture = current_texture_}
    );
}

void Model::DoRendering() {
    renderer_.GetRenderPort()->OnEvent({.scene = scene_, .camera = camera_});
}

void Model::MoveCamera(const CameraChange& camera_change) {
    camera_.Move(camera_change.move.direct_move, camera_change.move.horizon_move, camera_change.move.vertical_move);
    camera_.Rotate(
        camera_change.rotate.yaw_rotation, camera_change.rotate.pitch_rotation, camera_change.rotate.roll_rotation
    );
}

void Model::Refresh(FloatType delta_time) {
    current_delta_time_ = delta_time;
    animator_registry_.GetRefreshPort()->OnEvent(delta_time);
}

void Model::OnRenderedTexture(const TextureData& texture) {
    current_texture_ = texture;
    out_draw_event_port_->Notify({.delta_time = current_delta_time_, .render_texture = texture});
}

}  // namespace null_engine::tests
