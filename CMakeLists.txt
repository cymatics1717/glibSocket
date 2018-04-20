cmake_minimum_required(VERSION 3.10)
project(glibDemo C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(GIO gio-2.0)
pkg_check_modules(NOTIFY libnotify)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GIO_INCLUDE_DIRS})
include_directories(${NOTIFY_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLIB_LIBRARIES} ${GIO_LIBRARIES} ${NOTIFY_LIBRARIES})

add_executable(udpserver udpserver.c)
target_link_libraries(udpserver ${LIBS})

add_executable(tcpserver tcpserver.c)
target_link_libraries(tcpserver ${LIBS})
