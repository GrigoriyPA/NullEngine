#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/util/geometry/constants.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

struct CenteringMouseSettings {
    FloatType sensitivity = 0.001;
};

class CenteringMouseControl {
public:
    struct CameraChange {
        FloatType yaw_rotation = 0.0;
        FloatType pitch_rotation = 0.0;
    };

    explicit CenteringMouseControl(sf::RenderWindow& window, const CenteringMouseSettings& settings = {});

    InPort<sf::Event>* GetEventsPort();

    void SubscribeOnCameraChange(InPort<CameraChange>* observer_port) const;

private:
    void OnEvent(const sf::Event& event) const;

    void CenteringMouse() const;

    sf::RenderWindow& window_;
    CenteringMouseSettings settings_;
    int32_t window_width_;
    int32_t window_height_;
    InPort<sf::Event> in_events_port_;
    OutPort<CameraChange>::Ptr out_camera_change_port_ = OutPort<CameraChange>::Make();
};

}  // namespace null_engine
