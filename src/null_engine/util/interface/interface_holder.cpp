#include <null_engine/util/interface/interface_holder.hpp>

#include <null_engine/util/geometry/constants.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

namespace null_engine::util {

//// InterfaceHolder

InterfaceHolder& InterfaceHolder::AddObject(InterfaceObject::Ptr object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

void InterfaceHolder::Update() {
    FloatType delta_time = static_cast<FloatType>(timer_.restart().asSeconds());
    for (const auto& object : objects_) {
        object->Update(delta_time);
    }
}

void InterfaceHolder::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& object : objects_) {
        target.draw(*object, states);
    }
}

}  // namespace null_engine::util
