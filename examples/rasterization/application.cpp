#include "application.hpp"

#include "controller.hpp"
#include "model.hpp"
#include "view.hpp"

namespace null_engine::tests {

namespace {

constexpr uint64_t kViewWidth = 1000;
constexpr uint64_t kViewHeight = 1000;

}  // anonymous namespace

class NativeApplication::Impl {
public:
    Impl()
        : window_(sf::VideoMode(kViewWidth, kViewHeight), "Native quick start example")
        , controller_(window_)
        , model_(kViewWidth, kViewHeight)
        , view_(window_) {
        out_refresh_port_->Subscribe(controller_.GetRefreshPort());
        out_refresh_port_->Subscribe(model_.GetRefreshPort());
        out_refresh_port_->Subscribe(view_.GetRefreshPort());

        controller_.SubscribeToMouseMove(model_.GetMouseMovePort());
        model_.SubscribeToTextures(view_.GetTexturePort());
    }

    void Run() {
        sf::Clock timer;
        while (window_.isOpen()) {
            out_refresh_port_->Notify(timer.restart().asSeconds());
        }
    }

private:
    sf::RenderWindow window_;
    Controller controller_;
    Model model_;
    View view_;
    OutPort<FloatType>::Ptr out_refresh_port_ = OutPort<FloatType>::Make();
};

NativeApplication::NativeApplication()
    : impl_(std::make_unique<NativeApplication::Impl>()) {
}

NativeApplication::~NativeApplication() {
}

void NativeApplication::Run() {
    assert(impl_ && "Application is not initialised");

    impl_->Run();
}

}  // namespace null_engine::tests
