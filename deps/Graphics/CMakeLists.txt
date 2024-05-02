file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(Graphics STATIC ${SRCS})

target_include_directories(Graphics PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Graphics PRIVATE SPDLOG GLAD GLM GLFW ImGUI Logger) # Graphics specfically needs vendor access