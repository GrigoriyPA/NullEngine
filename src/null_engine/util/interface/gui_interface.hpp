#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <memory>
#include <vector>

namespace null_engine {

class GuiInterface : public sf::Drawable {
public:
    GuiInterface& AddObject(std::unique_ptr<sf::Drawable> object);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::unique_ptr<sf::Drawable>> objects_;
};

}  // namespace null_engine
