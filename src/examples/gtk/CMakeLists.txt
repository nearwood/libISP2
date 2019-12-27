cmake_minimum_required(VERSION 3.2)
project(libISP2_gtk)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

set(SOURCE_FILES main.cpp)
add_executable(libISP2_gtk ${SOURCE_FILES})

if(GTK_FOUND)
  message("GTK Found")
  include_directories(${GTK_INCLUDE_DIRS})
  target_link_libraries(libISP2_gtk ${GTK_LIBRARIES})
  add_definitions(${GTK_CFLAGS_OTHER})
endif()

target_link_libraries(libISP2_gtk ISP2)

install(TARGETS libISP2_gtk DESTINATION ${ISP2_INSTALL_BIN_DIR})
