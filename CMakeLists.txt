#cmake version
cmake_minimum_required(VERSION 3.20)


#3rd part libraries folder
set(VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

set(CMAKE_TOOLCHAIN_FILE "${VENDOR_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

#define the project
project(CPP_TEMPLATE VERSION 0.1)

#set cxx standard
add_library(cpptemplate_compiler_flags INTERFACE)
target_compile_features(cpptemplate_compiler_flags INTERFACE cxx_std_20)


set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(cpptemplate_compiler_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE: -W3>>"
    )

#Where static and shared libs are built
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory(Core)

configure_file(Config.h.in Config.h)

add_executable(CPP_TEMPLATE main.cpp)

target_include_directories(CPP_TEMPLATE PUBLIC
    "${PROJECT_BINARY_DIR}"
    )     

target_link_libraries(CPP_TEMPLATE PUBLIC cpptemplate_compiler_flags)

find_package(OpenGL REQUIRED)
target_link_libraries(CPP_TEMPLATE PRIVATE OpenGL::GL)

find_package(glfw CONFIG REQUIRED)

target_link_libraries(CPP_TEMPLATE PRIVATE OpenGL::GL)
target_link_libraries(CPP_TEMPLATE PRIVATE glfw3)
