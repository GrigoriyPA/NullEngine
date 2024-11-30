#pragma once

#include "interface_object.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Clock.hpp>

#include <vector>

namespace null_engine::util {

//
// Holder for all interface objects, provides time measuring
//
class InterfaceHolder : public sf::Drawable {
public:
    InterfaceHolder() = default;

    InterfaceHolder& AddObject(InterfaceObject::Ptr object);

public:
    // Update state of all interface objects.
    // Should be called before each draw
    void Update();

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<InterfaceObject::Ptr> objects_;
    sf::Clock timer_;
};

}  // namespace null_engine::util
