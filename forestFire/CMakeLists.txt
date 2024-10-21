cmake_minimum_required(VERSION 2.8)
project(AEForestFireProject)

option(TARGET_MIC "compile for Intel Xeon Phi")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -fopenmp -std=c++11")

if (TARGET_MIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmic")
endif()

SET(SOURCE_FILES main.cpp)

find_package(Boost REQUIRED system)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(fire ${SOURCE_FILES})

include_directories(fire_lib)

add_subdirectory(fire_lib)
add_subdirectory(fire_tests)

target_link_libraries(fire fire_lib)
