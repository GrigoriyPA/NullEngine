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
    const Vec3 square_size(1.0, 1.0);
    const Vec3 first_square_pos(-0.5, -0.5, 5.0);
    scene.AddObject(CreatePointsSet(number_points, first_square_pos, square_size, kWhite));

    const Vec3 second_square_pos(0.0, 0.0, 4.5);
    scene.AddObject(CreatePointsSet(number_points, second_square_pos, square_size, kRed));

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
    , camera_control_(camera_)
    , renderer_({view_width, view_height}) {
    in_refresh_port_->SetEventsHandler(std::bind(&Model::OnRefresh, this, std::placeholders::_1));

    out_refresh_port_->Subscribe(camera_control_.GetRefreshPort());
    out_render_port_->Subscribe(renderer_.GetRenderPort());
}

InPort<FloatType>* Model::GetRefreshPort() const {
    return in_refresh_port_.get();
}

InPort<Vec2>* Model::GetMouseMovePort() const {
    return camera_control_.GetMouseMovePort();
}

void Model::SubscribeToTextures(InPort<TextureData>* observer_port) const {
    renderer_.SubscribeToTextures(observer_port);
}

void Model::OnRefresh(FloatType delta_time) const {
    out_refresh_port_->Notify(delta_time);
    out_render_port_->Notify({.scene = scene_, .camera = camera_});
}

}  // namespace null_engine::tests
