#include <null_engine/util/interface/fonts.hpp>

namespace null_engine::util {

//// Fonts functions

FontPtr LoadFont(const std::string& file) {
    auto font = std::make_shared<sf::Font>();

    font->loadFromFile(file);

    return font;
}

}  // namespace null_engine::util
