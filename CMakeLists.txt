cmake_minimum_required (VERSION 3.1)

project(color)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wconversion -Wno-unused-private-field -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
 
find_package(PythonLibs REQUIRED 3)
find_package(PythonInterp REQUIRED 3)
find_package(NumPy REQUIRED)

add_library(color SHARED src/debug.cpp src/conversion.cpp src/CIEDE2000.cpp src/fitness.cpp)
target_include_directories(color PUBLIC include PRIVATE ${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIR})

add_executable(testCIEDE2000 test/testCIEDE2000.cpp)
target_link_libraries(testCIEDE2000 color)

add_executable(evo1 test/example1.cpp)
target_link_libraries(evo1 color)
add_executable(evo2 test/example2.cpp)
target_link_libraries(evo2 color)

add_executable(testlexi test/testlexi.cpp)
target_link_libraries(testlexi color)