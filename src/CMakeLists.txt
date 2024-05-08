add_executable(${PROJECT_NAME} 
	main.cpp 
	glad.c
)
target_link_libraries(${PROJECT_NAME} PRIVATE deps vendor)

configure_file(Config.h.in Config.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SRCS})

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)