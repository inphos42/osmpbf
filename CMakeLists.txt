#   This file is part of the osmpbf library.
#
#   Copyright(c) 2014 Oliver Groß.
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 3 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, see
#   <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(osmpbf-suite)
set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(PACKAGE_VERSION 0.2)

if(CMAKE_CROSSCOMPILING AND ANDROID)
	set(LIBRARY_OUTPUT_PATH_ROOT "${CMAKE_BINARY_DIR}")
endif(CMAKE_CROSSCOMPILING AND ANDROID)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/osmpbf/include)

add_subdirectory(osmpbf)

add_subdirectory(examples EXCLUDE_FROM_ALL)

