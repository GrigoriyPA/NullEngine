#pragma once

#include <SFML/Graphics/Font.hpp>

#include <memory>
#include <string>

namespace null_engine::util {

using FontPtr = std::shared_ptr<sf::Font>;

FontPtr LoadFont(const std::string& file);

}  // namespace null_engine::util
