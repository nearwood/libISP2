cmake_minimum_required(VERSION 3.2)
project(libISP2 C CXX)

set(SOURCE_FILES
    isp2.cpp
)

add_library(ISP2 SHARED STATIC ${SOURCE_FILES})

install(TARGETS ISP2 DESTINATION ${ISP2_INSTALL_LIB_DIR})
install(FILES isp2.h DESTINATION ${ISP2_INSTALL_INCLUDE_DIR})

add_subdirectory(example)
