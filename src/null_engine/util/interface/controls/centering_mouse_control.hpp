#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <null_engine/util/geometry/vector2.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

class CenteringMouseControl {
public:
    explicit CenteringMouseControl(sf::RenderWindow& window);

    InPort<sf::Event>* GetEventsPort() const;

    void SubscribeOnMouseMove(InPort<Vec2>* observer_port) const;

private:
    void OnEvent(const sf::Event& event) const;

    void CenteringMouse() const;

    sf::RenderWindow& window_;
    int32_t window_width_;
    int32_t window_height_;
    InPort<sf::Event>::Ptr in_events_port_ = InPort<sf::Event>::Make();
    OutPort<Vec2>::Ptr out_mouse_move_port_ = OutPort<Vec2>::Make();
};

}  // namespace null_engine
