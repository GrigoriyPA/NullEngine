#include "interface_holder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace null_engine::util {

InterfaceHolder& InterfaceHolder::AddObject(std::unique_ptr<sf::Drawable> object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

void InterfaceHolder::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& object : objects_) {
        target.draw(*object, states);
    }
}

}  // namespace null_engine::util
