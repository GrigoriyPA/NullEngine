#include "fonts.hpp"

namespace null_engine::util {

sf::Font LoadFont(const std::string& file) {
    sf::Font font;
    font.loadFromFile(file);

    return font;
}

}  // namespace null_engine::util
