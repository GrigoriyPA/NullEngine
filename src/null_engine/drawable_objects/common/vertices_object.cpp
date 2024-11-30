#include <null_engine/drawable_objects/common/vertices_object.hpp>

#include <null_engine/generic/mesh/generic_vertex.hpp>

#include <null_engine/util/generic/validation.hpp>
#include <null_engine/util/geometry/vector_3d.hpp>

#include <sstream>

namespace null_engine::drawable {

//// VerticesObject

VerticesObject::VerticesObject(uint64_t number_vertices)
    : vertices_(number_vertices, generic::Vertex(util::Vec3(0.0), util::Vec3(1.0)))
    , indices_(number_vertices) {
    for (size_t i = 0; i < indices_.size(); ++i) {
        indices_[i] = i;
    }
}

VerticesObject& VerticesObject::SetVertex(uint64_t index, const generic::Vertex& vertex) {
    util::Ensure(
        index < vertices_.size(), std::stringstream() << "Invalid vertex index " << index << " for vertex object with "
                                                      << vertices_.size() << " vertices"
    );

    vertices_[index] = vertex;
    return *this;
}

VerticesObject& VerticesObject::SetPositions(const std::vector<util::Vec3>& positions) {
    util::Ensure(
        positions.size() == vertices_.size(), std::stringstream()
                                                  << "Invalid positions size " << positions.size()
                                                  << " for vertex object with " << vertices_.size() << " vertices"
    );

    for (size_t i = 0; i < vertices_.size(); ++i) {
        vertices_[i].SetPosition(positions[i]);
    }
    return *this;
}

VerticesObject& VerticesObject::SetColors(const std::vector<util::Vec3>& colors) {
    util::Ensure(
        colors.size() == vertices_.size(), std::stringstream()
                                               << "Invalid colors size " << colors.size() << " for vertex object with "
                                               << vertices_.size() << " vertices"
    );

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
        util::Ensure(
            index < vertices_.size(), std::stringstream() << "Invalid index " << index << " for vertex object with "
                                                          << vertices_.size() << " vertices"
        );
    }

    indices_ = indices;
    return *this;
}

const std::vector<generic::Vertex>& VerticesObject::GetVertices() const {
    return vertices_;
}

const std::vector<uint64_t>& VerticesObject::GetIndices() const {
    return indices_;
}

namespace tests {

//// Test functions

VerticesObject::Ptr CreatePointsSet(uint64_t number_points, util::Vec3 offset, util::Vec3 size, util::Vec3 color) {
    auto object = VerticesObject::Make(number_points * number_points);

    util::Vec3 step = size / static_cast<util::FloatType>(number_points);
    for (uint64_t i = 0; i < number_points; ++i) {
        for (uint64_t j = 0; j < number_points; ++j) {
            const util::Vec3 point =
                offset + step * util::Vec3(static_cast<util::FloatType>(i), static_cast<util::FloatType>(j));

            object->SetVertex(i * number_points + j, generic::Vertex(point, color));
        }
    }

    return object;
}

}  // namespace tests

}  // namespace null_engine::drawable
