cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(jsonish
	VERSION 0.0.0
	DESCRIPTION "Parser for jsonish"
	LANGUAGES CXX)

set(JSONISH_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(JSONISH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)

option(JSONISH_BUILD_TESTS "Build jsonish tests" ON)
if (JSONISH_BUILD_TESTS)
	add_subdirectory(tests)
endif()
