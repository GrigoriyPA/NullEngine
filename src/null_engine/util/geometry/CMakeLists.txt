set(TARGET_NAME null-engine-util-geometry)

add_library(${TARGET_NAME}
    helpers.cpp
    line.cpp
    matrix.cpp
    vector.cpp
)

target_link_libraries(${TARGET_NAME}
    assimp::assimp
    Boost::compute
    Eigen3::Eigen
    null-engine-util-generic
)
