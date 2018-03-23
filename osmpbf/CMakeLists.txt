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

set(OSMPBF_LIBRARIES
	${PROJECT_NAME}
	CACHE STRING "osmpbf libraries"
	FORCE
)

set(MY_LINK_LIBRARIES
	${PROTOBUF_LIBRARIES}
	${ZLIB_LIBRARIES}
)

set(OSMPBF_LINK_LIBRARIES
	${PROJECT_NAME}
	${MY_LINK_LIBRARIES}
	CACHE STRING "osmpbf link libraries"
	FORCE
)

mark_as_advanced(
	OSMPBF_INCLUDE_DIRS
	OSMPBF_LIBRARIES
	OSMPBF_LINK_LIBRARIES
)

include_directories(
	${OSMPBF_INCLUDE_DIRS}
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
	abstractnodeinputadaptor.cpp
	primitiveblockoutputadaptor.cpp
	nodestreaminputadaptor.cpp
	wayinputadaptor.cpp
	relationinputadaptor.cpp
	pbistream.cpp
	oway.cpp
	onode.cpp
	filter.cpp
	xmlconverter.cpp
	dataindex.cpp
	fileio.cpp
	net.cpp
)

# fetch all include headers
file(GLOB_RECURSE INCLUDE_HEADERS include/*.h)

add_library(${PROJECT_NAME} STATIC
	${PROTO_SOURCES} ${PROTO_HEADERS}
	${SOURCES_CPP} ${INCLUDE_HEADERS}
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${MY_LINK_LIBRARIES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
