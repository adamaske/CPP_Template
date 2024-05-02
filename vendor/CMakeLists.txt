add_library(vendor INTERFACE)

add_subdirectory(Eigen)
add_subdirectory(GLM)
add_subdirectory(GLFW)
add_subdirectory(GLAD)
add_subdirectory(ImGUI)
add_subdirectory(GLEW)
add_subdirectory(JSON)
add_subdirectory(SPDLOG)

target_link_libraries(vendor INTERFACE
    Eigen
    GLM
    GLFW
    GLAD
    GLEW
    ImGUI
    JSON
    SPDLOG
)