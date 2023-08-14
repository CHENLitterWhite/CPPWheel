cmake_minimum_required(VERSION 3.0.0)
project(CPPWheel VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_COMPILE_FEATURES "g++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${PROJECT_SOURCE_DIR}/SsdpUdpGroup/include)

aux_source_directory("${PROJECT_SOURCE_DIR}/SsdpUdpGroup/src" SOURCES)
aux_source_directory("${PROJECT_SOURCE_DIR}" SOURCES)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
add_executable(SsdpUdpGroup ${SOURCES})