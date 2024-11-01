cmake_minimum_required(VERSION 3.20)

project(HypergraphKCore)

if(APPLE)
#    you can use g++ to get better debugging information
#    set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-13")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
else ()
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# helpful compiler flags for gcc/clang
add_compile_options(
        -Wall
        -Wextra
        -Werror
        -pedantic-errors
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wcast-align
        -Wformat=2
        -Wuninitialized
        -Wnull-dereference
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wold-style-cast
        -Wzero-as-null-pointer-constant
        -Wno-self-assign-overloaded
        -Wno-self-move
        -Wsuggest-override
        -fstack-protector-strong
)


add_executable(HypergraphKCore
        src/main.cpp
        src/Hypergraph.cpp
        src/Hypergraph.h
        src/Algorithm/NbrCD.h
        src/Algorithm/baseNbrCoreDecomposition.cpp
        src/Algorithm/Final_nbrCoreDecomposition.cpp
        src/Algorithm/ParNbrCoreDecomposition.cpp
        src/Algorithm/baseNbrCoreDecomposition_withNbrs.cpp
        src/Algorithm/fullNbrCoreDecomposition_withNbrs.cpp
        src/Algorithm/Final_nbrCoreDecomposition_withNbrs.cpp
        src/Algorithm/fullNbrCoreDecomposition.cpp
        src/Algorithm/Final_nbrCoreDecomposition_onlyChanegd.cpp
        src/Algorithm/Final_nbrCoreDecomposition_lowerBound.cpp
        src/Algorithm/Final_nbrCoreDecomposition_no_filter.cpp


        src/Algorithm_withInfo/NbrCD.h
        src/Algorithm_withInfo/baseNbrCoreDecomposition.cpp
        src/Algorithm_withInfo/Final_nbrCoreDecomposition.cpp
        src/Algorithm_withInfo/ParNbrCoreDecomposition.cpp
        src/Algorithm_withInfo/baseNbrCoreDecomposition_withNbrs.cpp
        src/Algorithm_withInfo/fullNbrCoreDecomposition_withNbrs.cpp
        src/Algorithm_withInfo/Final_nbrCoreDecomposition_withNbrs.cpp
        src/Algorithm_withInfo/fullNbrCoreDecomposition.cpp
        src/Algorithm_withInfo/Final_nbrCoreDecomposition_onlyChanegd.cpp
        src/Algorithm_withInfo/Final_nbrCoreDecomposition_lowerBound.cpp
        src/Algorithm_withInfo/Final_nbrCoreDecomposition_no_filter.cpp

        src/graphTool/graphTool.cpp
        src/graphTool/graphTool.h
        src/graphTool/sortGraph.cpp
        src/graphTool/generateGraph.cpp
)
