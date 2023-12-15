#cmake version
cmake_minimum_required(VERSION 3.20)

#define the project
project(CPP_TEMPLATE VERSION 0.1)

#set cxx standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(Config.h.in Config.h)

add_executable(CPP_TEMPLATE main.cpp)

target_include_directories(CPP_TEMPLATE PUBLIC
                            "${PROJECT_BINARY_DIR}"
                        )           