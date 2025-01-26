#include "gui_interface.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace null_engine {

GuiInterface& GuiInterface::AddObject(std::unique_ptr<sf::Drawable> object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

void GuiInterface::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& object : objects_) {
        target.draw(*object, states);
    }
}

}  // namespace null_engine
