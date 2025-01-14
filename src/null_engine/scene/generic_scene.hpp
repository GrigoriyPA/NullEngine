#pragma once

#include <folly/Poly.h>

#include <null_engine/generic/mesh/generic_mesh_interface.hpp>
#include <vector>

namespace null_engine {

class Scene {
public:
    Scene() = default;

    const std::vector<folly::Poly<IMeshObject>>& GetObjects() const;

    Scene& AddObject(folly::Poly<IMeshObject> object);

private:
    std::vector<folly::Poly<IMeshObject>> objects_;
};

}  // namespace null_engine
