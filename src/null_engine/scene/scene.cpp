#include "scene.hpp"

namespace null_engine {

const std::vector<VerticesObject>& Scene::GetObjects() const {
    return objects_;
}

Scene& Scene::AddObject(VerticesObject object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

}  // namespace null_engine
