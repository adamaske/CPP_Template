file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(Core STATIC ${SRCS})

target_include_directories(Core PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)
