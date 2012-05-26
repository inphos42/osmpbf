cmake_minimum_required(VERSION 2.8)

project(osmpbf)

find_package(Protobuf REQUIRED)
find_package(ZLIB REQUIRED)

set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=gnu++0x")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}")

include_directories(
	${PROTOBUF_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
)

set(SOURCES_CPP
	blobfile.cpp
	primitiveblockinputadaptor.cpp
	iway.cpp
	inode.cpp
	primitiveblockoutputadaptor.cpp
	oway.cpp
	stringtable.cpp
)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS osmblob.proto osmformat.proto)

add_library(${PROJECT_NAME} STATIC ${PROTO_SOURCES} ${PROTO_HEADERS} ${SOURCES_CPP})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES})
