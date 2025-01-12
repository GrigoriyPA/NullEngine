#include "interface_object.hpp"

namespace null_engine::util {

void InterfaceObject::Update(FloatType delta_time) {
}

void InterfaceObject::OnEvent(const sf::Event& event) {
}

void InterfaceObject::draw(sf::RenderTarget& target, sf::RenderStates states) const {
}

}  // namespace null_engine::util
