﻿# CMakeList.txt : CMake project for sim-aos, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.8)

project ("CA-sim")

set (CMAKE_CXX_STANDARD 20)

if (MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
else()
    add_compile_options(-Wall -Wextra -Wno-deprecated -Werror -pedantic -pedantic-errors)
endif()

# Add openMP
find_package(OpenMP REQUIRED)


# Add source to this project's executable.
add_executable (sim-aos "sim-aos/sim-aos.cpp" "sim-aos/sim-aos.h" "sim-aos/object.h")
add_executable (sim-soa "sim-soa/sim-soa.cpp" "sim-soa/sim-soa.h" "sim-soa/object.h")

add_executable (sim-paos "sim-paos/sim-paos.cpp" "sim-paos/sim-paos.h" "sim-paos/object.h" "common/watch.h" "common/pair.h")
add_executable (sim-psoa "sim-psoa/sim-psoa.cpp" "sim-psoa/sim-psoa.h" "sim-psoa/object.h" "common/watch.h" "common/pair.h")
target_link_libraries (sim-paos PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries (sim-psoa PUBLIC OpenMP::OpenMP_CXX)
