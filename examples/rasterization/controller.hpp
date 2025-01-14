#pragma once

#include <null_engine/util/interface/controls/centering_mouse_control.hpp>

namespace null_engine::tests {

class Controller {
public:
    explicit Controller(sf::RenderWindow& window);

    InPort<FloatType>* GetRefreshPort() const;

    void SubscribeToMouseMove(InPort<Vec2>* observer_port) const;

private:
    void OnRefresh(FloatType delta_time) const;

    sf::RenderWindow& window_;
    CenteringMouseControl mouse_control_;
    InPort<FloatType>::Ptr in_refresh_port_ = InPort<FloatType>::Make();
    OutPort<sf::Event>::Ptr out_events_port_ = OutPort<sf::Event>::Make();
};

}  // namespace null_engine::tests
