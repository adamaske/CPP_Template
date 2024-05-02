add_library(SPDLOG INTERFACE)#header only

target_include_directories(SPDLOG INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
