cmake_minimum_required(VERSION 3.2)

project(libISP2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#For installs
set(ISP2_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(ISP2_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(ISP2_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(ISP2_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/ISP2)

include_directories(${ISP2_INSTALL_INCLUDE_DIR})
include_directories(${ISP2_HEADERS_DIR})

add_subdirectory(src)
#add_subdirectory(test)
