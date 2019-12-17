cmake_minimum_required(VERSION 3.2)
project(libISP2_example)

set(SOURCE_FILES isp2.cpp)

add_executable(libISP2_example ${SOURCE_FILES})
target_link_libraries(libISP2_example ISP2)
install(TARGETS libISP2_example DESTINATION ${ISP2_INSTALL_BIN_DIR})
