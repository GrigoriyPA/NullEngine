#include "fonts.hpp"

namespace null_engine {

std::unique_ptr<sf::Font> LoadFont(const std::string& file) {
    auto font = std::make_unique<sf::Font>();
    font->loadFromFile(file);

    return font;
}

}  // namespace null_engine
