cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(osmpbf-suite)
set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(PACKAGE_VERSION 0.2)

if(CMAKE_CROSSCOMPILING AND ANDROID)
	set(LIBRARY_OUTPUT_PATH_ROOT "${CMAKE_BINARY_DIR}")
endif(CMAKE_CROSSCOMPILING AND ANDROID)

add_subdirectory(osmpbf)

add_subdirectory(examples)
add_dependencies(dumppbf osmpbf)
add_dependencies(dumppbf_openmp osmpbf)
