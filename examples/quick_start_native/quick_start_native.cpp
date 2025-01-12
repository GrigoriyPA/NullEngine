#include <folly/Poly.h>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <null_engine/drawable_objects/common/vertices_object.hpp>
#include <null_engine/generic/camera/generic_camera_control.hpp>
#include <null_engine/generic/camera/generic_direct_camera.hpp>
#include <null_engine/generic/generic_scene.hpp>
#include <null_engine/generic/generic_scene_renderer.hpp>
#include <null_engine/generic/renderer/generic_texture_consumer.hpp>
#include <null_engine/native/native_renderer.hpp>
#include <null_engine/tests/tests_helpers.hpp>
#include <null_engine/util/interface/helpers/events.hpp>
#include <null_engine/util/interface/helpers/timer.hpp>
#include <null_engine/util/interface/interface_holder.hpp>
#include <null_engine/util/interface/objects/fps_counter.hpp>

namespace null_engine::example {

class QuickStartNativeApplicaton {
    static constexpr uint64_t kViewWidth = 1000;
    static constexpr uint64_t kViewHeight = 1000;

public:
    QuickStartNativeApplicaton()
        : font_(util::LoadFont("../../assets/fonts/arial.ttf"))
        , window_(sf::VideoMode(kViewWidth, kViewHeight), "Native quick start example")
        , rendering_consumer_(kViewWidth, kViewHeight)
        , scene_()
        , scene_renderer_(
              native::Renderer(native::RendererSettings{.view_width = kViewWidth, .view_height = kViewHeight}),
              rendering_consumer_
          )
        , camera_(10.0, 10.0, 10.0)
        , events_provider_(util::CreateEventsProvider())
        , timer_provider_(util::CreateTimerProvider())
        , camera_controller_(camera_, window_, timer_provider_, events_provider_)
        , interface_() {
        FillScene();
        FillInterface();
    }

    void Run() {
        while (window_.isOpen()) {
            for (sf::Event event; window_.pollEvent(event);) {
                if (event.type == sf::Event::Closed ||
                    (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
                    window_.close();
                    return;
                }

                events_provider_.DispatchEvent(event);
            }

            timer_provider_.RefreshClients();
            window_.clear();

            scene_renderer_.Render(scene_, camera_);
            window_.draw(rendering_consumer_);
            window_.draw(interface_);

            window_.display();
        }
    }

private:
    void FillScene() {
        scene_
            .AddObject(tests::CreatePointsSet(200, util::Vec3(-0.5, -0.5, 5.0), util::Vec3(1.0, 1.0), util::Vec3(1.0)))
            .AddObject(
                tests::CreatePointsSet(200, util::Vec3(0.0, 0.0, 4.5), util::Vec3(1.0, 1.0), util::Vec3(1.0, 0.0, 0.0))
            );
    }

    void FillInterface() {
        interface_.AddObject(util::FPSCounter::Make(0.5, font_, timer_provider_));
    }

private:
    sf::Font font_;
    sf::RenderWindow window_;
    generic::WindowRenderingConsumer rendering_consumer_;
    generic::Scene scene_;
    generic::SceneRenderer scene_renderer_;
    generic::DirectCamera camera_;
    folly::Poly<util::IEventsProvider> events_provider_;
    folly::Poly<util::ITimerProvider> timer_provider_;
    generic::SimpleCameraControl camera_controller_;
    util::InterfaceHolder interface_;
};

}  // namespace null_engine::example

int main() {
    try {
        null_engine::example::QuickStartNativeApplicaton application;
        application.Run();
    } catch (const std::exception& error) {
        std::cerr << "Got unexpected exception:\n" << error.what();
    } catch (...) {
        std::cerr << "Got unexpected exception\n";
    }

    return 0;
}
