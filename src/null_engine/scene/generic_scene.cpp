#include "generic_scene.hpp"

#include <null_engine/util/generic/validation.hpp>

namespace null_engine {

const std::vector<folly::Poly<IMeshObject>>& Scene::GetObjects() const {
    return objects_;
}

Scene& Scene::AddObject(folly::Poly<IMeshObject> object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

}  // namespace null_engine
