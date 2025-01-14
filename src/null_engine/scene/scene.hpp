#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>

namespace null_engine {

class Scene {
public:
    Scene() = default;

    const std::vector<VerticesObject>& GetObjects() const;

    Scene& AddObject(VerticesObject object);

private:
    std::vector<VerticesObject> objects_;
};

}  // namespace null_engine
