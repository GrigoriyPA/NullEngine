#pragma once

#include <SFML/Graphics/Font.hpp>
#include <memory>

namespace null_engine {

std::unique_ptr<sf::Font> LoadFont(const std::string& file);

}  // namespace null_engine
