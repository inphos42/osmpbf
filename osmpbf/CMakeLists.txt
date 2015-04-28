#   This file is part of the osmpbf library.
#
#   Copyright(c) 2012-2014 Oliver Groß.
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
	osmfilein.cpp
	abstractprimitiveinputadaptor.cpp
	primitiveblockinputadaptor.cpp
	iway.cpp
	inode.cpp
	irelation.cpp
	iinfo.cpp
	abstractprimitiveoutputadaptor.cpp
	primitiveblockoutputadaptor.cpp
	nodestreaminputadaptor.cpp
	wayinputadaptor.cpp
	relationinputadaptor.cpp
	oway.cpp
	onode.cpp
	filter.cpp
	xmlconverter.cpp
	dataindex.cpp
)

# fetch all include headers
file(GLOB_RECURSE INCLUDE_HEADERS include/*.h)

add_library(${PROJECT_NAME} STATIC
	${PROTO_SOURCES} ${PROTO_HEADERS}
	${SOURCES_CPP} ${INCLUDE_HEADERS}
)
