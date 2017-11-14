cmake_minimum_required (VERSION 3.1)

project(color)

# Compiler-specific C++11 activation.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(color SHARED src/color.cpp src/conversion.cpp src/CIEDE2000.cpp)
target_include_directories(color PUBLIC include)

add_executable(testCIEDE2000 test/testCIEDE2000.cpp)
target_link_libraries(testCIEDE2000 color)
