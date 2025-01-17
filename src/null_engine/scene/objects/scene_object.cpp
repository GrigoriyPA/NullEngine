#include "scene_object.hpp"

#include <cassert>

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
        child_it_ ? **child_it_ : RenderObject{.vetices_object = self_->GetObject(), .instances = {Mat4()}};

    const auto& animation_transform = self_->GetAnimationTransform();
    const auto& instances = self_->GetInstances();

    const size_t result_size = result.instances.size();
    result.instances.reserve(result_size * instances.size());
    for (size_t i = 0; i < instances.size(); ++i) {
        const auto transform = instances[i] * animation_transform;
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

SceneObject::SceneObject(const Mat4& instance)
    : instances_(1, instance) {
}

SceneObject::SceneObject(const VerticesObject& object, const Mat4& instance)
    : instances_(1, instance)
    , object_(object) {
}

bool SceneObject::HasObject() const {
    return object_.has_value();
}

const VerticesObject& SceneObject::GetObject() const {
    assert(object_ && "Can not get vertices object from empty scene object");
    return *object_;
}

const std::vector<Mat4>& SceneObject::GetInstances() const {
    return instances_;
}

Mat4 SceneObject::GetAnimationTransform() const {
    return animation_ ? animation_->GetTransform() : Mat4();
}

size_t SceneObject::GetNumberChildren() const {
    return children_.size();
}

const SceneObject& SceneObject::GetChild(size_t child_id) const {
    assert(child_id < children_.size() && "Child id too large");
    return children_[child_id];
}

SceneObject& SceneObject::AddInstance(const Mat4& instance) {
    instances_.emplace_back(instance);
    return *this;
}

SceneObject& SceneObject::SetAnimation(Animation::Ptr animation) {
    animation_ = std::move(animation);
    return *this;
}

SceneObject& SceneObject::AddChildren(SceneObject object) {
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
