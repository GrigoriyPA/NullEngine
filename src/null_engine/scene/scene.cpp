#include "scene.hpp"

#include <cassert>

namespace null_engine {

Scene::Iterator::Iterator(const Scene* self, size_t index)
    : self_(self)
    , index_(index) {
    UpdateObjectIt();
}

bool Scene::Iterator::operator==(const Iterator& other) const {
    return self_ == other.self_ && index_ == other.index_ && object_it_ == other.object_it_;
}

Scene::Iterator& Scene::Iterator::operator++() {
    if (!object_it_) {
        ++index_;
        return *this;
    }

    ++(*object_it_);
    if (!object_it_->IsEnd()) {
        return *this;
    }

    ++index_;
    UpdateObjectIt();
    return *this;
}

RenderObject Scene::Iterator::operator*() const {
    assert(object_it_ && "Can not dereference invalid scene iterator");
    return **object_it_;
}

void Scene::Iterator::UpdateObjectIt() {
    if (index_ < self_->GetNumberObjects()) {
        object_it_ = self_->GetObject(index_).begin();
    } else {
        object_it_ = std::nullopt;
    }
}

size_t Scene::GetNumberObjects() const {
    return objects_.size();
}

size_t Scene::GetNumberLights() const {
    return lights_.size();
}

const SceneObject& Scene::GetObject(size_t index) const {
    assert(index < objects_.size() && "Scene object index too large");
    return objects_[index];
}

const std::vector<SceneObject>& Scene::GetObjects() const {
    return objects_;
}

AnyLight Scene::GetLight(size_t index) const {
    assert(index < lights_.size() && "Scene light index too large");
    return lights_[index]->GetLight();
}

std::vector<AnyLight> Scene::GetLights() const {
    std::vector<AnyLight> result;
    result.reserve(lights_.size());
    for (const auto& light : lights_) {
        if (light->Enabled()) {
            result.emplace_back(light->GetLight());
        }
    }
    return result;
}

Scene& Scene::AddObject(const VerticesObject& object) {
    objects_.emplace_back(object, Mat4());
    return *this;
}

Scene& Scene::AddObject(SceneObject object) {
    objects_.emplace_back(std::move(object));
    return *this;
}

Scene& Scene::AddLight(const AnyMovableLight& light) {
    lights_.emplace_back(SceneLight::Make(light));
    return *this;
}

Scene& Scene::AddLight(SceneLight::Ptr light) {
    lights_.emplace_back(std::move(light));
    return *this;
}

Scene::Iterator Scene::begin() const {
    return Iterator(this, 0);
}

Scene::Iterator Scene::end() const {
    return Iterator(this, objects_.size());
}

}  // namespace null_engine
