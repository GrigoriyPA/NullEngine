#pragma once

#include <null_engine/renderer/camera/camera_control.hpp>
#include <null_engine/renderer/renderer.hpp>

namespace null_engine::tests {

class Model {
public:
    using TextureData = std::vector<uint8_t>;

    Model(uint64_t view_width, uint64_t view_height);

    InPort<FloatType>* GetRefreshPort() const;

    InPort<Vec2>* GetMouseMovePort() const;

    void SubscribeToTextures(InPort<TextureData>* observer_port) const;

private:
    void OnRefresh(FloatType delta_time) const;

    Scene scene_;
    AnyMovableCamera camera_;
    CameraControl camera_control_;
    Renderer renderer_;
    InPort<FloatType>::Ptr in_refresh_port_ = InPort<FloatType>::Make();
    OutPort<FloatType>::Ptr out_refresh_port_ = OutPort<FloatType>::Make();
    OutPort<RenderEvent>::Ptr out_render_port_ = OutPort<RenderEvent>::Make();
};

}  // namespace null_engine::tests
