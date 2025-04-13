#include "scene_object.hpp"

namespace null_engine {

SceneObject::Iterator::Iterator(const SceneObject* self, size_t object_id, size_t child_id)
    : self_(self)
    , object_id_(object_id)
    , child_id_(child_id) {
    if (child_id_ < self->GetNumberChildren()) {
        child_it_ = std::make_unique<Iterator>(self_->GetChild(child_id_).begin());
        UpdateChildIt();
    }
}

bool SceneObject::Iterator::operator==(const Iterator& other) const {
    return self_ == other.self_ && object_id_ == other.object_id_ && child_id_ == other.child_id_ &&
           child_it_ == other.child_it_;
}

SceneObject::Iterator& SceneObject::Iterator::operator++() {
    if (object_id_ < self_->GetNumberObjects()) {
        ++object_id_;
        return *this;
    }

    if (child_it_) {
        ++(*child_it_);
        UpdateChildIt();
    }

    return *this;
}

RenderObject SceneObject::Iterator::operator*() const {
    RenderObject result = object_id_ < self_->GetNumberObjects()
                              ? RenderObject{.vetices_object = self_->GetObject(object_id_), .instances = {Ident()}}
                              : **child_it_;

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
    return object_id_ >= self_->GetNumberObjects() && child_id_ >= self_->GetNumberChildren();
}

void SceneObject::Iterator::UpdateChildIt() {
    while (child_it_->IsEnd()) {
        if (++child_id_ >= self_->GetNumberChildren()) {
            child_it_ = nullptr;
            return;
        }
        child_it_ = std::make_unique<Iterator>(self_->GetChild(child_id_).begin());
    }
}

SceneObject::SceneObject(const Transform& instance)
    : instances_(1, instance) {
}

SceneObject::SceneObject(const VerticesObject& object, const Transform& instance)
    : instances_(1, instance)
    , objects_(1, object) {
}

InPort<Transform>* SceneObject::GetTransformPort() {
    return transform_->GetInPort();
}

SceneObject::Statistic SceneObject::GetStatistic() const {
    Statistic result = {.number_objects = objects_.size()};
    for (const auto& object : objects_) {
        const auto& object_statistic = object.GetStatistic();
        result.number_points += object_statistic.number_points;
        result.number_faces += object_statistic.number_faces;
    }

    for (const auto& child : children_) {
        const auto& child_statistic = child.GetStatistic();
        result.number_points += child_statistic.number_points;
        result.number_faces += child_statistic.number_faces;
        result.number_objects += child_statistic.number_objects;
        result.max_depth = std::max(result.max_depth, child_statistic.max_depth + 1);
    }

    return result;
}

size_t SceneObject::GetNumberObjects() const {
    return objects_.size();
}

const VerticesObject& SceneObject::GetObject(size_t object_id) const {
    assert(object_id < objects_.size() && "Object id too large");
    return objects_[object_id];
}

const std::vector<VerticesObject>& SceneObject::GetObjects() const {
    return objects_;
}

size_t SceneObject::GetNumberInstances() const {
    return instances_.size();
}

Transform SceneObject::GetInstance(size_t instance_id) const {
    assert(instance_id < instances_.size() && "Instance id too large");
    return instances_[instance_id];
}

const std::vector<Transform>& SceneObject::GetInstances() const {
    return instances_;
}

size_t SceneObject::GetNumberChildren() const {
    return children_.size();
}

const SceneObject& SceneObject::GetChild(size_t child_id) const {
    assert(child_id < children_.size() && "Child id too large");
    return children_[child_id];
}

const std::vector<SceneObject>& SceneObject::GetChildren() const {
    return children_;
}

Transform SceneObject::GetTransform() const {
    return transform_->GetState().value_or(Ident());
}

SceneObject& SceneObject::AddObject(const VerticesObject& object) {
    objects_.emplace_back(object);
    return *this;
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
    return SceneObject::Iterator(this, 0, 0);
}

SceneObject::Iterator SceneObject::end() const {
    return SceneObject::Iterator(this, objects_.size(), children_.size());
}

}  // namespace null_engine
