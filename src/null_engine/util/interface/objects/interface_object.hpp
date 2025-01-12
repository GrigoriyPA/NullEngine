#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <null_engine/util/geometry/constants.hpp>

namespace null_engine::util {

//
// Common interface for all drawable interface objects
//
class InterfaceObject : public sf::Drawable {
public:
    using Ptr = std::shared_ptr<InterfaceObject>;

public:
    virtual void Update(FloatType delta_time);

    virtual void OnEvent(const sf::Event& event);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}  // namespace null_engine::util
