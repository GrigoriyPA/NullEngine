#include "quick_start_application.hpp"

#include <null_engine/generic/camera/generic_camera_control.hpp>
#include <null_engine/generic/camera/generic_direct_camera.hpp>
#include <null_engine/generic/generic_scene_renderer.hpp>
#include <null_engine/generic/renderer/generic_texture_consumer.hpp>
#include <null_engine/native/native_renderer.hpp>
#include <null_engine/tests/tests_constants.hpp>
#include <null_engine/tests/tests_helpers.hpp>
#include <null_engine/util/interface/helpers/events.hpp>
#include <null_engine/util/interface/interface_holder.hpp>
#include <null_engine/util/interface/objects/fps_counter.hpp>

namespace null_engine::example {

namespace {

constexpr uint64_t kViewWidth = 1000;
constexpr uint64_t kViewHeight = 1000;
constexpr const char* kFontPath = "../../assets/fonts/arial.ttf";

struct Providers {
    folly::Poly<util::IEventsProvider> events;
    folly::Poly<util::ITimerProvider> timer;
};

generic::Scene CreateScene() {
    static constexpr uint64_t kNumberPoints = 200;

    return generic::Scene()
        .AddObject(
            tests::CreatePointsSet(kNumberPoints, util::Vec3(-0.5, -0.5, 5.0), util::Vec3(1.0, 1.0), tests::kWhite)
        )
        .AddObject(tests::CreatePointsSet(kNumberPoints, util::Vec3(0.0, 0.0, 4.5), util::Vec3(1.0, 1.0), tests::kRed));
}

util::InterfaceHolder CreateInterface(const sf::Font& font, Providers& providers) {
    return util::InterfaceHolder().AddObject(util::FPSCounter(0.5, font, providers.timer));
}

folly::Poly<generic::IMovableCamera> CreateCamera() {
    static constexpr util::FloatType kCameraBoxSize = 10.0;

    return generic::DirectCamera(kCameraBoxSize, kCameraBoxSize, kCameraBoxSize);
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
    folly::Poly<generic::IMovableCamera> camera_;
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
