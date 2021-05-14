cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)
project(benchmark)

# include CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

# extlib
set(EXTLIB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extlib")
set(EXTLIB_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/extlib")

# compiler
set(GCC_MIN_VERSION "10.1")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_MIN_VERSION)
        message(FATAL_ERROR "GCC version must be at least ${GCC_MIN_VERSION}!")
    else()
        message(STATUS "Using GCC version ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
endif()

# build flags
set(CXX_STANDARD c++20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++20 ${GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")

# build type
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not defined - defaulting to Debug.")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "CMAKE_BUILD_TYPE is Release")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "CMAKE_BUILD_TYPE is RelWithDebInfo")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "CMAKE_BUILD_TYPE is Debug")
else()
    message(FATAL_ERROR "Unsupported CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# init submodules
include(InitSubmodules)

# include extlibs
add_subdirectory(${EXTLIB_SOURCE_DIR}/tlx)
include_directories(${EXTLIB_SOURCE_DIR}/tlx)

# include
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# benchmarks
add_executable(lz78 lz78.cpp)
target_link_libraries(lz78 tlx)
