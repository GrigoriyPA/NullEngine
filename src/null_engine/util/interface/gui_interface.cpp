#include "gui_interface.hpp"

namespace null_engine {

InterfaceHolder& InterfaceHolder::AddObject(AnyInterfaceObject object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

void InterfaceHolder::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& object : objects_) {
        object.draw(target, states);
    }
}

}  // namespace null_engine
