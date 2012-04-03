cmake_minimum_required(VERSION 2.6)

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

project(osmpbf)

find_package(Protobuf REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(
	${PROTOBUF_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
)

set(SOURCES_CPP
	osmpbf.cpp
	main.cpp
)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS osmblob.proto osmformat.proto)

add_executable(osmpbf ${PROTO_SOURCES} ${PROTO_HEADERS} ${SOURCES_CPP})
target_link_libraries(osmpbf ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES})

add_target_properties(osmpbf COMPILE_FLAGS "-std=c++0x")
