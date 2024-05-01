file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(ImGUI STATIC ${SRCS})

target_include_directories(ImGUI PUBLIC ${INCS}
${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ImGUI PRIVATE GLFW GLEW)