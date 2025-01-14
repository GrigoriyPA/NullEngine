#pragma once

#include <null_engine/util/interface/objects/object_interface.hpp>

namespace null_engine {

class InterfaceHolder : public sf::Drawable {
public:
    InterfaceHolder() = default;

    InterfaceHolder& AddObject(AnyInterfaceObject object);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<AnyInterfaceObject> objects_;
};

}  // namespace null_engine
