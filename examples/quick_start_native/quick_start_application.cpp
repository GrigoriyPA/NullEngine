#include "quick_start_application.hpp"

#include <null_engine/generic/camera/generic_camera_control.hpp>
#include <null_engine/generic/camera/generic_perspective_camera.hpp>
#include <null_engine/generic/generic_scene_renderer.hpp>
#include <null_engine/generic/renderer/generic_texture_consumer.hpp>
#include <null_engine/native/native_renderer.hpp>
#include <null_engine/tests/tests_constants.hpp>
#include <null_engine/tests/tests_helpers.hpp>
#include <null_engine/util/interface/helpers/events.hpp>
#include <null_engine/util/interface/interface_holder.hpp>
#include <null_engine/util/interface/objects/fps_counter.hpp>
#include <numbers>

namespace null_engine::example {

namespace {

constexpr uint64_t kViewWidth = 1000;
constexpr uint64_t kViewHeight = 1000;
constexpr const char* kFontPath = "../../assets/fonts/arial.ttf";

using Camera = folly::Poly<generic::IMovableCamera>;

struct Providers {
    folly::Poly<util::IEventsProvider> events;
    folly::Poly<util::ITimerProvider> timer;
};

generic::Scene CreateScene() {
    generic::Scene scene;

    const uint64_t number_points = 200;
    const util::Vec3 square_size(1.0, 1.0);
    const util::Vec3 first_square_pos(-0.5, -0.5, 5.0);
    scene.AddObject(tests::CreatePointsSet(number_points, first_square_pos, square_size, tests::kWhite));

    const util::Vec3 second_square_pos(0.0, 0.0, 4.5);
    scene.AddObject(tests::CreatePointsSet(number_points, second_square_pos, square_size, tests::kRed));

    return scene;
}

util::InterfaceHolder CreateInterface(const sf::Font& font, Providers& providers) {
    util::InterfaceHolder interface;

    const util::FloatType update_period = 0.5;
    interface.AddObject(util::FPSCounter(update_period, font, providers.timer));

    return interface;
}

Camera CreateCamera() {
    const util::FloatType fov = std::numbers::pi / 2.0;
    const util::FloatType min_distance = 0.1;
    const util::FloatType max_distance = 250;
    return generic::PerspectiveCamera(
        fov, static_cast<util::FloatType>(kViewWidth) / static_cast<util::FloatType>(kViewHeight), min_distance,
        max_distance
    );
}

}  // anonymous namespace

class NativeApplication::Impl {
public:
    Impl()
        : providers_({.events = util::CreateEventsProvider(), .timer = util::CreateTimerProvider()})
        , font_(util::LoadFont(kFontPath))
        , window_(sf::VideoMode(kViewWidth, kViewHeight), "Native quick start example")
        , scene_(CreateScene())
        , interface_(CreateInterface(font_, providers_))
        , camera_(CreateCamera()) {
    }

    void Run() {
        generic::SimpleCameraControl camera_controller(camera_, window_, providers_.timer, providers_.events);

        generic::WindowRenderingConsumer rendering_window(kViewWidth, kViewHeight);
        generic::SceneRenderer scene_renderer(
            native::Renderer(native::RendererSettings{.view_width = kViewWidth, .view_height = kViewHeight}),
            rendering_window
        );

        while (window_.isOpen()) {
            for (sf::Event event; window_.pollEvent(event);) {
                if (event.type == sf::Event::Closed ||
                    (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
                    window_.close();
                    return;
                }

                providers_.events.DispatchEvent(event);
            }
            providers_.timer.RefreshClients();

            window_.clear();

            scene_renderer.Render(scene_, camera_);
            window_.draw(rendering_window);
            window_.draw(interface_);

            window_.display();
        }
    }

private:
    Providers providers_;
    sf::Font font_;
    sf::RenderWindow window_;
    generic::Scene scene_;
    util::InterfaceHolder interface_;
    Camera camera_;
};

NativeApplication::NativeApplication()
    : impl_(std::make_unique<NativeApplication::Impl>()) {
}

NativeApplication::~NativeApplication() {
}

void NativeApplication::Run() {
    impl_->Run();
}

}  // namespace null_engine::example
