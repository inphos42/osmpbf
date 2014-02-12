#   This file is part of the osmpbf library.
#
#   Copyright(c) 2012-2013 Oliver Groß.
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

project(osmpbf)

MACRO(ADD_TARGET_PROPERTIES _target _name)
	SET(_properties)
	FOREACH(_prop ${ARGN})
		SET(_properties "${_properties} ${_prop}")
	ENDFOREACH(_prop)
	GET_TARGET_PROPERTY(_old_properties ${_target} ${_name})
	MESSAGE(STATUS "adding property to ${_target} ${_name}:" ${_properties})
	IF(NOT _old_properties)
		# in case it's NOTFOUND
		SET(_old_properties)
	ENDIF(NOT _old_properties)
	SET_TARGET_PROPERTIES(${_target} PROPERTIES ${_name} "${_old_properties} ${_properties}")
ENDMACRO(ADD_TARGET_PROPERTIES)

set(DEBUG_FLAGS " ${DEBUG_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}")

if(CMAKE_CROSSCOMPILING)
	# find build host protoc and include generator function(s)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
	find_package(Protobuf)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

	set(PROTOBUF_INCLUDE_DIRS ${ANDROID_EXT_PATH_ROOT}/include)
else(CMAKE_CROSSCOMPILING)
	find_package(Protobuf)
endif(CMAKE_CROSSCOMPILING)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS osmblob.proto osmformat.proto)

find_package(ZLIB REQUIRED)

include_directories(
	${PROTOBUF_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
)

set(SOURCES_CPP
	blobfile.cpp
	osmfile.cpp
	abstractprimitiveinputadaptor.cpp
	primitiveblockinputadaptor.cpp
	iway.cpp
	inode.cpp
	irelation.cpp
	abstractprimitiveoutputadaptor.cpp
	primitiveblockoutputadaptor.cpp
	oway.cpp
	onode.cpp
	filter.cpp
)

add_library(${PROJECT_NAME} STATIC ${PROTO_SOURCES} ${PROTO_HEADERS} ${SOURCES_CPP})

add_executable(dumppbf dumppbf.cpp)
target_link_libraries(dumppbf ${PROJECT_NAME} ${ZLIB_LIBRARIES} ${PROTOBUF_LIBRARIES})

add_executable(dumppbf_openmp dumppbf.cpp)
target_link_libraries(dumppbf_openmp ${PROJECT_NAME} ${ZLIB_LIBRARIES} ${PROTOBUF_LIBRARIES})
add_target_properties(dumppbf_openmp COMPILE_FLAGS "-fopenmp")
add_target_properties(dumppbf_openmp LINK_FLAGS "-fopenmp")


# target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES} ${PROTOBUF_LIBRARIES})
