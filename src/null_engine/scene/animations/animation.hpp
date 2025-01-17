#pragma once

#include <null_engine/util/geometry/matrix4.hpp>
#include <null_engine/util/mvc/ports.hpp>

namespace null_engine {

class Animation {
public:
    using Ptr = std::unique_ptr<Animation>;

    Animation();

    static Animation::Ptr Make();

    InPort<Mat4>* GetTransformPort();

    const Mat4& GetTransform() const;

    bool Initialized() const;

private:
    void OnUpdateTransform(const Mat4& transform);

    Mat4 transform_;
    InPort<Mat4> in_transform_port_;
};

}  // namespace null_engine
