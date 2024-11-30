#include <null_engine/drawable_objects/common/vertices_object.hpp>

#include <null_engine/generic/camera/generic_direct_camera.hpp>
#include <null_engine/generic/renderer/generic_texture_consumer.hpp>
#include <null_engine/generic/generic_scene.hpp>

#include <null_engine/native/native_renderer.hpp>

#include <null_engine/util/geometry/vector_3d.hpp>
#include <null_engine/util/interface/fonts.hpp>
#include <null_engine/util/interface/fps_counter.hpp>
#include <null_engine/util/interface/interface_holder.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <iostream>

namespace null_engine::example {

constexpr uint64_t kViewWidth = 500;
constexpr uint64_t kViewHeight = 500;

generic::Scene InitScene(sf::Texture& output_texture) {
    auto scene =
        generic::Scene()
            .AddObject(drawable::tests::CreatePointsSet(
                300, util::Vec3(-0.5, -0.5, 1.0), util::Vec3(1.0, 1.0), util::Vec3(1.0)
            ))
            .AddObject(drawable::tests::CreatePointsSet(
                300, util::Vec3(0.0, 0.0, 0.5), util::Vec3(1.0, 1.0), util::Vec3(1.0, 0.0)
            ))
            .SetDefaultRenderer(
                native::Renderer::Make(native::RendererSettings{.view_width = kViewWidth, .view_height = kViewHeight})
            );

    scene.AddCamera(generic::DirectCamera::Make(2.0, 2.0, 2.0))
        .AddConsumer(generic::TextureRenderingConsumer::Make(output_texture));

    return scene;
}

util::InterfaceHolder InitInterface() {
    auto interface =
        util::InterfaceHolder().AddObject(util::FPSCounter::Make(0.5, util::LoadFont("../../assets/fonts/arial.ttf")));

    return interface;
}

void RunExample() {
    // SFML initialization

    sf::RenderWindow window(sf::VideoMode(kViewWidth, kViewHeight), "Native quick start example");

    sf::Texture texture;
    texture.create(kViewWidth, kViewHeight);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    // NE initialization

    auto scene = InitScene(texture);
    auto interface = InitInterface();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        scene.Render();
        interface.Update();

        window.clear();
        window.draw(sprite);
        window.draw(interface);
        window.display();
    }
}

}  // namespace null_engine::example

int main() {
    try {
        null_engine::example::RunExample();
    } catch (const std::exception& error) {
        std::cerr << "Got unexpected exception:\n" << error.what();
    } catch (...) {
        std::cerr << "Got unexpected exception\n";
    }

    return 0;
}
