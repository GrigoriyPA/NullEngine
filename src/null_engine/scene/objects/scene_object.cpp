#include "scene_object.hpp"

namespace null_engine {

SceneObject::Iterator::Iterator(const SceneObject* self, size_t index)
    : self_(self)
    , index_(index) {
    UpdateChildIt();
}

bool SceneObject::Iterator::operator==(const Iterator& other) const {
    return self_ == other.self_ && index_ == other.index_ && child_it_ == other.child_it_;
}

SceneObject::Iterator& SceneObject::Iterator::operator++() {
    if (!child_it_) {
        ++index_;
        return *this;
    }

    ++(*child_it_);
    if (!child_it_->IsEnd()) {
        return *this;
    }

    ++index_;
    UpdateChildIt();
    return *this;
}

RenderObject SceneObject::Iterator::operator*() const {
    RenderObject result =
        child_it_ ? **child_it_ : RenderObject{.vetices_object = self_->GetObject(), .instances = {Ident()}};

    const auto& object_transform = self_->GetTransform();
    const auto& instances = self_->GetInstances();

    const size_t result_size = result.instances.size();
    result.instances.reserve(result_size * instances.size());
    for (size_t i = 0; i < instances.size(); ++i) {
        const auto transform = instances[i] * object_transform;
        for (size_t r = 0; r < result_size; ++r) {
            auto& result_transform = result.instances[r];
            if (i + 1 < instances.size()) {
                result.instances.emplace_back(transform * result_transform);
            } else {
                result_transform = transform * result_transform;
            }
        }
    }

    return result;
}

bool SceneObject::Iterator::IsEnd() const {
    if (child_it_) {
        return false;
    }

    const auto number_children = self_->GetNumberChildren();
    if (index_ < number_children) {
        return false;
    }

    return !self_->HasObject() || index_ > number_children;
}

void SceneObject::Iterator::UpdateChildIt() {
    if (index_ < self_->GetNumberChildren()) {
        child_it_ = std::make_unique<Iterator>(self_->GetChild(index_).begin());
    } else {
        child_it_ = nullptr;
    }
}

SceneObject::SceneObject(const Transform& instance)
    : instances_(1, instance) {
}

SceneObject::SceneObject(const VerticesObject& object, const Transform& instance)
    : instances_(1, instance)
    , object_(object) {
}

InPort<Transform>* SceneObject::GetTransformPort() {
    return transform_->GetInPort();
}

bool SceneObject::HasObject() const {
    return object_.has_value();
}

const VerticesObject& SceneObject::GetObject() const {
    assert(object_ && "Can not get vertices object from empty scene object");
    return *object_;
}

const std::vector<Transform>& SceneObject::GetInstances() const {
    return instances_;
}

Transform SceneObject::GetTransform() const {
    return transform_->GetState().value_or(Ident());
}

size_t SceneObject::GetNumberChildren() const {
    return children_.size();
}

const SceneObject& SceneObject::GetChild(size_t child_id) const {
    assert(child_id < children_.size() && "Child id too large");
    return children_[child_id];
}

SceneObject& SceneObject::AddInstance(const Transform& instance) {
    instances_.emplace_back(instance);
    return *this;
}

SceneObject& SceneObject::AddChild(SceneObject object) {
    children_.emplace_back(std::move(object));
    return *this;
}

SceneObject::Iterator SceneObject::begin() const {
    return SceneObject::Iterator(this, 0);
}

SceneObject::Iterator SceneObject::end() const {
    return SceneObject::Iterator(this, children_.size() + HasObject());
}

}  // namespace null_engine
