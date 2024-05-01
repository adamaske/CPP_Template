file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(Logger STATIC ${SRCS})

target_include_directories(Logger PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Logger PRIVATE ImGUI)