cmake_minimum_required (VERSION 3.1)

project(perception)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -Wno-unused-private-field -Wno-unused-variable -Wno-unused-parameter -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wconversion -Wno-unused-private-field -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIB_SOURCES src/colorspace.cpp src/CIEDE2000.cpp src/fitness.cpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(PythonLibsNew REQUIRED)  # -DPYTHON_EXECUTABLE should be set


add_library(native MODULE ${LIB_SOURCES} src/python.cpp) 
target_link_libraries(native ${PYTHON_LIBRARIES})
target_include_directories(native PRIVATE include ${PYTHON_INCLUDE_DIR})
set_target_properties(native PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}")
set_target_properties(native PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}")

if(CMAKE_BUILD_TYPE MATCHES "Debug")

    add_library(perception SHARED ${LIB_SOURCES}) 
    target_include_directories(perception PUBLIC include)

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

endif(CMAKE_BUILD_TYPE MATCHES "Debug")
