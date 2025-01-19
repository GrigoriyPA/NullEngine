#pragma once

#include <null_engine/scene/animations/animation.hpp>
#include <null_engine/util/mvc/ports.hpp>

#include "light_interface.hpp"

namespace null_engine {

enum class SceneLightEvent { TurnOn, TurnOf };

class SceneLight {
public:
    using Ptr = std::unique_ptr<SceneLight>;

    explicit SceneLight(const AnyMovableLight& light);

    static SceneLight::Ptr Make(const AnyMovableLight& light);

    InPort<Mat4>* GetTransformPort();

    InPort<SceneLightEvent>* GetEventsPort();

    bool Enabled() const;

    AnyLight GetLight() const;

private:
    void OnEvent(SceneLightEvent event);

    AnyMovableLight light_;
    bool enabled_ = true;
    Animation animation_;
    InPort<SceneLightEvent> in_events_port_;
};

}  // namespace null_engine
