#pragma once

#include <folly/Poly.h>

#include <null_engine/util/interface/objects/object_interface.hpp>
#include <vector>

namespace null_engine::util {

class InterfaceHolder : public sf::Drawable {
public:
    InterfaceHolder() = default;

    InterfaceHolder& AddObject(folly::Poly<IInterfaceObject> object);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<folly::Poly<IInterfaceObject>> objects_;
};

}  // namespace null_engine::util
