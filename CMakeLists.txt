cmake_minimum_required (VERSION 3.1)
project (FuelGauge)
include(gtest.cmake)

########################################################################
# Compiler specific setup
########################################################################
set (CMAKE_C_STANDARD 99)
if(CMAKE_COMPILER_IS_GNUCC AND NOT WIN32)
    ADD_DEFINITIONS(-Werror)
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-W)
    ADD_DEFINITIONS(-Wextra)
    ADD_DEFINITIONS(-Wno-unused-parameter)
    ADD_DEFINITIONS(-Wsign-compare)
endif()

# The main library
add_subdirectory (lib)

# The unit tests
enable_testing()
add_subdirectory (test)
