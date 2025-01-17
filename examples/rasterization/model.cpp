#include "model.hpp"

#include <null_engine/renderer/camera/camera.hpp>
#include <null_engine/tests/tests_constants.hpp>
#include <null_engine/tests/tests_helpers.hpp>
#include <numbers>

namespace null_engine::tests {

namespace {

Scene CreateScene() {
    Scene scene;

    const uint64_t number_points = 200;
    const Vec2 square_size(1.0, 1.0);
    const Vec3 first_square_pos(-0.5, -0.5, 5.0);
    scene.AddObject(CreatePointsSet(number_points, first_square_pos, square_size, kWhite));

    const Vec3 second_square_pos(0.0, 0.0, 4.5);
    scene.AddObject(CreatePointsSet(number_points, second_square_pos, square_size, kRed));

    const std::vector<Vec3> sample_points = {
        Vec3(-0.5, -0.5, 0.5), Vec3(0.5, -0.7, 0.5), Vec3(0, 0.5, 0.5), Vec3(0.5, 0.5, 1.5)
    };
    const std::vector<VertexParams> sample_params = {
        VertexParams{.color = kRed}, VertexParams{.color = kGreen}, VertexParams{.color = kBlue},
        VertexParams{.color = kWhite}
    };

    scene.AddObject(VerticesObject(sample_points.size(), VerticesObject::Type::LineLoop)
                        .SetPositions(sample_points)
                        .SetParams(sample_params));

    const Vec3 traingles_translation(2.0, 0.0, 2.0);
    scene.AddObject(VerticesObject(sample_points.size(), VerticesObject::Type::TriangleStrip)
                        .SetPositions(sample_points)
                        .SetParams(sample_params)
                        .Transform(Mat4::Translation(traingles_translation)));

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
    : scene_(CreateScene())
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
}

void Model::OnRenderedTexture(const TextureData& texture) {
    current_texture_ = texture;
    out_draw_event_port_->Notify({.delta_time = current_delta_time_, .render_texture = texture});
}

}  // namespace null_engine::tests
