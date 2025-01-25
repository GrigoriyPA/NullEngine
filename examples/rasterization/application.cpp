#include "application.hpp"

#include <SFML/Window/Event.hpp>

#include "controller.hpp"
#include "model.hpp"
#include "view.hpp"

namespace null_engine::tests {

namespace {

constexpr uint64_t kViewWidth = 800;
constexpr uint64_t kViewHeight = 800;
constexpr bool kMultithreadRendering = true;

}  // anonymous namespace

class Application::Impl {
public:
    Impl()
        : window_(sf::VideoMode(kViewWidth, kViewHeight), "Rasterisation example")
        , model_(kViewWidth, kViewHeight, kMultithreadRendering)
        , controller_(window_, &model_)
        , view_(window_) {
        model_.SubscribeToDrawEvents(view_.GetDrawEventsPort());
    }

    void Run() {
        sf::Clock timer;
        while (window_.isOpen()) {
            for (sf::Event event; window_.pollEvent(event);) {
                if (event.type == sf::Event::Closed ||
                    (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
                    window_.close();
                    return;
                }

                controller_.AddEvent(event);
            }
            controller_.AddRefresh(timer.restart().asSeconds());
            controller_.Apply();
        }
    }

private:
    sf::RenderWindow window_;
    Model model_;
    Controller controller_;
    View view_;
};

Application::Application()
    : impl_(std::make_unique<Application::Impl>()) {
}

Application::~Application() = default;

void Application::Run() {
    impl_->Run();
}

}  // namespace null_engine::tests
