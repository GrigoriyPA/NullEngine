#include "interface_holder.hpp"

namespace null_engine::util {

InterfaceHolder& InterfaceHolder::AddObject(folly::Poly<IInterfaceObject> object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

void InterfaceHolder::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& object : objects_) {
        object.draw(target, states);
    }
}

}  // namespace null_engine::util
