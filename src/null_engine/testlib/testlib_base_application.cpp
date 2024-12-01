#include <null_engine/testlib/testlib_base_application.hpp>

namespace null_engine::testlib {

//// BaseApplication

util::WindowPtr BaseApplication::GetWindow() const {
    return window_;
}

void BaseApplication::Run() {
    SetUp();

    while (window_->isOpen()) {
        for (sf::Event event; window_->pollEvent(event);) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
                window_->close();
                break;
            }

            interface_.HandleEvent(event);
        }

        interface_.Update();
        window_->clear();

        scene_.Render();
        Draw();

        window_->display();
    }
}

void BaseApplication::SetUp() {
    window_ = CreateWindow();
    scene_ = CreateScene();
    interface_ = CreateInterface();
}

void BaseApplication::Draw() {
    window_->draw(interface_);
}

}  // namespace null_engine::testlib
