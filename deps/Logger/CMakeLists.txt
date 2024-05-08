file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(Logger STATIC ${SRCS})

target_include_directories(Logger PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Logger INTERFACE "USE_SPDLOG")# Propagate this to the main compiler
target_link_libraries(Logger PRIVATE SPDLOG)
#find_library(THIRD_PARTY_LOGGER SPDLOG vendor)
#if(THIRD_PARTY_LOGGER)
#	target_compile_definitions(Logger INTERFACE "USE_SPDLOG")
#	target_link_libraries(Logger PRIVATE SPDLOG)
#endif()
