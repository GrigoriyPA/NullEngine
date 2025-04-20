#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>
#include <null_engine/util/mvc/observer.hpp>
#include <vector>

namespace null_engine {

struct RenderObject {
    const VerticesObject& vetices_object;
    std::vector<Transform> instances;
};

class SceneObject {
public:
    class Iterator {
        friend class SceneObject;

        Iterator(const SceneObject* self, size_t object_id, size_t child_id);

    public:
        bool operator==(const Iterator& other) const;

        Iterator& operator++();

        RenderObject operator*() const;

        bool IsEnd() const;

    private:
        void UpdateChildIt();

        const SceneObject* self_;
        size_t object_id_ = 0;
        size_t child_id_ = 0;
        std::unique_ptr<Iterator> child_it_;
    };

    struct Statistic {
        uint64_t number_points = 0;
        uint64_t number_faces = 0;
        uint64_t number_objects = 0;
        uint64_t max_depth = 1;
    };

    explicit SceneObject(const Transform& instance);

    explicit SceneObject(const VerticesObject& object, const Transform& instance = Ident());

    InPort<Transform>* GetTransformPort();

    Statistic GetStatistic() const;

    size_t GetNumberObjects() const;

    const VerticesObject& GetObject(size_t object_id) const;

    const std::vector<VerticesObject>& GetObjects() const;

    size_t GetNumberInstances() const;

    Transform GetInstance(size_t instance_id) const;

    const std::vector<Transform>& GetInstances() const;

    size_t GetNumberChildren() const;

    const SceneObject& GetChild(size_t child_id) const;

    const std::vector<SceneObject>& GetChildren() const;

    Transform GetTransform() const;

    SceneObject& AddObject(const VerticesObject& object);

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
    Observer<Transform>::Ptr transform_ = Observer<Transform>::Make();
    std::vector<Transform> instances_;
    std::vector<VerticesObject> objects_;
    std::vector<SceneObject> children_;
};

}  // namespace null_engine
