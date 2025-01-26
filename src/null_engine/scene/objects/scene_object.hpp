#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/mvc/observer.hpp>

namespace null_engine {

struct RenderObject {
    const VerticesObject& vetices_object;
    std::vector<Transform> instances;
};

class SceneObject {
public:
    class Iterator {
        friend class SceneObject;

        Iterator(const SceneObject* self, size_t index);

    public:
        bool operator==(const Iterator& other) const;

        Iterator& operator++();

        RenderObject operator*() const;

        bool IsEnd() const;

    private:
        void UpdateChildIt();

        const SceneObject* self_;
        size_t index_;
        std::unique_ptr<Iterator> child_it_;
    };

    explicit SceneObject(const Transform& instance);

    explicit SceneObject(const VerticesObject& object, const Transform& instance = Ident());

    InPort<Transform>* GetTransformPort();

    bool HasObject() const;

    const VerticesObject& GetObject() const;

    const std::vector<Transform>& GetInstances() const;

    Transform GetTransform() const;

    size_t GetNumberChildren() const;

    const SceneObject& GetChild(size_t child_id) const;

    SceneObject& AddInstance(const Transform& instance);

    SceneObject& AddChild(SceneObject object);

    template <typename... Args>
    SceneObject& EmplaceChild(Args&&... args) {
        children_.emplace_back(std::forward<Args>(args)...);
        return *this;
    }

    Iterator begin() const;

    Iterator end() const;

private:
    std::vector<Transform> instances_;
    Observer<Transform>::Ptr transform_ = Observer<Transform>::Make();
    std::optional<VerticesObject> object_;
    std::vector<SceneObject> children_;
};

}  // namespace null_engine
