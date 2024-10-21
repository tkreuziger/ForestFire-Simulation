project(FireLibrary)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -fopenmp -std=c++11 -qopt-report=5 -qopt-report-phase=vec")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

set(SOURCE_FILES model.cpp tree.cpp wind.cpp)
set(HEADER_FILES model.h tree.h wind.h)

find_package(Boost REQUIRED system)
include_directories(${Boost_INCLUDE_DIRS})

add_library(fire_lib STATIC  ${SOURCE_FILES} ${HEADER_FILES})
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
