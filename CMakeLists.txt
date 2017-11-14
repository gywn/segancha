cmake_minimum_required (VERSION 3.1)

project(color)

# Compiler-specific C++11 activation.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(CIEDE2000 SHARED src/CIEDE2000.cpp)
target_include_directories(CIEDE2000 PUBLIC include)

add_executable(testCIEDE2000 test/testCIEDE2000.cpp)
target_link_libraries(testCIEDE2000 CIEDE2000)
