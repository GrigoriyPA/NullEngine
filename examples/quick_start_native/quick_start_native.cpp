#include <null_engine/native/rasterization/native_rasterizer.hpp>
#include <null_engine/native/rasterization/native_rasterizer_context.hpp>

#include <geometry/vector_3d.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace null_engine::example {}  // namespace null_engine::example

int main() {
    const uint64_t view_height = 500;
    const uint64_t view_width = 500;

    null_engine::native::RasterizerContext context(view_width, view_height);
    null_engine::native::Rasterizer rasterizer(context);

    null_engine::native::tests::DrawPoints(rasterizer, 500, null_engine::util::Vec3(-0.5, -0.5),
                                           null_engine::util::Vec3(1.0, 1.0), null_engine::util::Vec3(1.0));

    null_engine::native::tests::DrawPoints(rasterizer, 500, null_engine::util::Vec3(0.0, 0.0, -0.5),
                                           null_engine::util::Vec3(1.0, 1.0), null_engine::util::Vec3(1.0, 0.0));

    sf::RenderWindow window(sf::VideoMode(view_width, view_height), "Native quick start example");

    sf::Texture texture;
    texture.create(view_width, view_height);
    texture.update(context.colors_buffer.data());

    sf::Sprite sprite;
    sprite.setTexture(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
