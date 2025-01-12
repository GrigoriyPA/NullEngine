#include <iostream>
#include <null_engine/drawable_objects/common/vertices_object.hpp>
#include <null_engine/generic/camera/generic_camera_control.hpp>
#include <null_engine/generic/camera/generic_direct_camera.hpp>
#include <null_engine/generic/renderer/generic_texture_consumer.hpp>
#include <null_engine/native/native_renderer.hpp>
#include <null_engine/testlib/testlib_base_application.hpp>
#include <null_engine/util/interface/objects/fps_counter.hpp>

namespace null_engine::example {

class QuickStartNativeApplicaton : public testlib::BaseApplication {
    static constexpr uint64_t kViewWidth = 1000;
    static constexpr uint64_t kViewHeight = 1000;

protected:
    util::WindowPtr CreateWindow() override {
        return std::make_shared<sf::RenderWindow>(sf::VideoMode(kViewWidth, kViewHeight), "Native quick start example");
    }

    generic::Scene CreateScene() override {
        // Add scene objects

        auto scene = generic::Scene()
                         .AddObject(drawable::tests::CreatePointsSet(
                             200, util::Vec3(-0.5, -0.5, 5.0), util::Vec3(1.0, 1.0), util::Vec3(1.0)
                         ))
                         .AddObject(drawable::tests::CreatePointsSet(
                             200, util::Vec3(0.0, 0.0, 4.5), util::Vec3(1.0, 1.0), util::Vec3(1.0, 0.0, 0.0)
                         ))
                         .SetDefaultRenderer(native::Renderer::Make(
                             native::RendererSettings{.view_width = kViewWidth, .view_height = kViewHeight}
                         ));

        // Add scene cameras

        camera_consumer_ = generic::WindowRenderingConsumer::Make(kViewWidth, kViewHeight);
        camera_ = generic::DirectCamera::Make(10.0, 10.0, 10.0);

        scene.AddCamera(camera_).AddConsumer(camera_consumer_);

        return scene;
    }

    util::InterfaceHolder CreateInterface() override {
        auto interface = util::InterfaceHolder()
                             .AddObject(camera_consumer_)
                             .AddObject(generic::SimpleCameraControl::Make(camera_, GetWindow()))
                             .AddObject(util::FPSCounter::Make(0.5, util::LoadFont("../../assets/fonts/arial.ttf")));

        return interface;
    }

private:
    generic::MovableCamera::Ptr camera_;
    generic::WindowRenderingConsumer::Ptr camera_consumer_;
};

}  // namespace null_engine::example

int main() {
    try {
        null_engine::example::QuickStartNativeApplicaton().Run();
    } catch (const std::exception& error) {
        std::cerr << "Got unexpected exception:\n" << error.what();
    } catch (...) {
        std::cerr << "Got unexpected exception\n";
    }

    return 0;
}
