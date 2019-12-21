cmake_minimum_required(VERSION 3.2)
project(libISP2_qt)

#find_package(Qt REQUIRED)

set(SOURCE_FILES qt.cpp)

add_executable(libISP2_qt ${SOURCE_FILES})
target_link_libraries(libISP2_qt ISP2)
target_link_libraries(libISP2_qt ${CURSES_LIBRARIES})
install(TARGETS libISP2_qt DESTINATION ${ISP2_INSTALL_BIN_DIR})
