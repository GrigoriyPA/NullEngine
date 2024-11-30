#include <null_engine/drawable_objects/common/vertices_object.hpp>

#include <null_engine/native/common/native_vertex.hpp>

#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

#include <sstream>

namespace null_engine::drawable {

//// VerticesObject

VerticesObject::VerticesObject(uint64_t number_vertices)
    : vertices_(number_vertices, native::Vertex(util::Vec3(0.0), util::Vec3(1.0)))
    , indices_(number_vertices) {
    for (size_t i = 0; i < indices_.size(); ++i) {
        indices_[i] = i;
    }
}

VerticesObject& VerticesObject::SetPositions(const std::vector<util::Vec3>& positions) {
    util::Ensure(positions.size() == vertices_.size(),
                 std::stringstream() << "Invalid positions size " << positions.size() << " for vertex object with "
                                     << vertices_.size() << " vertices");

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetPosition(positions[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColors(const std::vector<util::Vec3>& colors) {
    util::Ensure(colors.size() == vertices_.size(), std::stringstream() << "Invalid colors size " << colors.size()
                                                                        << " for vertex object with "
                                                                        << vertices_.size() << " vertices");

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetColor(colors[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColor(util::Vec3 color) {
    for (auto& vertex : vertices_) {
        vertex.SetColor(color);
    }
    return *this;
}

VerticesObject& VerticesObject::SetIndices(const std::vector<uint64_t>& indices) {
    for (const auto index : indices) {
        util::Ensure(index < vertices_.size(), std::stringstream()
                                                   << "Invalid index " << index << " for vertex object with "
                                                   << vertices_.size() << " vertices");
    }

    indices_ = indices;
    return *this;
}

}  // namespace null_engine::drawable
