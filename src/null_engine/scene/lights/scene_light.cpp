#include "scene_light.hpp"

namespace null_engine {

SceneLight::SceneLight(const AnyMovableLight& light)
    : light_(light)
    , in_events_port_(std::bind(&SceneLight::OnEvent, this, std::placeholders::_1)) {
}

SceneLight::Ptr SceneLight::Make(const AnyMovableLight& light) {
    return std::make_unique<SceneLight>(light);
}

InPort<Mat4>* SceneLight::GetTransformPort() {
    return animation_.GetTransformPort();
}

InPort<SceneLightEvent>* SceneLight::GetEventsPort() {
    return &in_events_port_;
}

bool SceneLight::Enabled() const {
    return enabled_;
}

AnyLight SceneLight::GetLight() const {
    auto result = light_;
    result.ApplyTransform(animation_.GetTransform());
    return result;
}

void SceneLight::OnEvent(SceneLightEvent event) {
    switch (event) {
        case SceneLightEvent::TurnOn:
            enabled_ = true;
            break;

        case SceneLightEvent::TurnOf:
            enabled_ = false;
            break;
    }
}

}  // namespace null_engine
