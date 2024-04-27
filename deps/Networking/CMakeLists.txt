file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(Networking STATIC ${SRCS})

target_include_directories(Networking PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)
