cmake_minimum_required(VERSION 3.2)
project(libISP2_ncurses)

find_package(Curses REQUIRED)

set(SOURCE_FILES isp2.cpp)

add_executable(libISP2_ncurses ${SOURCE_FILES})
target_link_libraries(libISP2_ncurses ISP2)
target_link_libraries(libISP2_ncurses ${CURSES_LIBRARIES})
install(TARGETS libISP2_ncurses DESTINATION ${ISP2_INSTALL_BIN_DIR})
