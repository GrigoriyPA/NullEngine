#pragma once

#include <null_engine/util/geometry/constants.hpp>

#include <SFML/Graphics/Drawable.hpp>

#include <memory>

namespace null_engine::util {

//
// Common interface for all drawable interface objects
//
class InterfaceObject : public sf::Drawable {
public:
    using Ptr = std::shared_ptr<InterfaceObject>;

public:
    // Update object state by given time delta.
    // Called before each drowing
    virtual void Update(FloatType delta_time) = 0;
};

}  // namespace null_engine::util
