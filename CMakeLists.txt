cmake_minimum_required (VERSION 3.1)

project(perception)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -Wno-unused-private-field -Wno-unused-variable -Wno-unused-parameter -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wconversion -Wno-unused-private-field -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
 
find_package(PythonLibs REQUIRED 3)
# find_package(PythonInterp REQUIRED 3)
# find_package(NumPy REQUIRED)

add_library(perception SHARED src/colorspace.cpp src/CIEDE2000.cpp src/fitness.cpp)
target_include_directories(perception PUBLIC include)

add_library(pyperception MODULE src/python.cpp) 
target_link_libraries(pyperception perception ${PYTHON_LIBRARIES})
target_include_directories(pyperception PRIVATE include ${PYTHON_INCLUDE_DIR})
set_target_properties(pyperception PROPERTIES PREFIX "")

add_executable(testevo1 test/example1.cpp)
target_link_libraries(testevo1 perception)
add_executable(testevo2 test/example2.cpp)
target_link_libraries(testevo2 perception)

add_executable(testlexi test/testlexi.cpp)
target_link_libraries(testlexi perception)

add_executable(testCIEDE2000 test/testCIEDE2000.cpp)
target_link_libraries(testCIEDE2000 perception)
add_executable(testcolorspace test/testcolorspace.cpp)
target_link_libraries(testcolorspace perception)
add_executable(testfitness test/testfitness.cpp)
target_link_libraries(testfitness perception)
add_executable(testfitness2 test/testfitness2.cpp)
target_link_libraries(testfitness2 perception)