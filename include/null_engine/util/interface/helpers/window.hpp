#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

namespace null_engine::util {

using WindowPtr = std::shared_ptr<sf::RenderWindow>;

}  // namespace null_engine::util
