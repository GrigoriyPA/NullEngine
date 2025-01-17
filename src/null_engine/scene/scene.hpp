#pragma once

#include <null_engine/scene/objects/scene_object.hpp>

namespace null_engine {

class Scene {
    class Iterator {
        friend class Scene;

        Iterator(const Scene* self, size_t index);

    public:
        bool operator==(const Iterator& other) const;

        Iterator& operator++();

        RenderObject operator*() const;

    private:
        void UpdateObjectIt();

        const Scene* self_;
        size_t index_;
        std::optional<SceneObject::Iterator> object_it_;
    };

public:
    Scene() = default;

    size_t GetNumberObjects() const;

    const SceneObject& GetObject(size_t index) const;

    Scene& AddObject(const VerticesObject& object);

    Scene& AddObject(SceneObject object);

    template <typename... Args>
    Scene& EmplaceObject(Args&&... args) {
        objects_.emplace_back(std::forward<Args>(args)...);
        return *this;
    }

    Iterator begin() const;

    Iterator end() const;

private:
    std::vector<SceneObject> objects_;
};

}  // namespace null_engine
