# The name of our project is "Pallet_Viewer"
# root source directory = ${PALLET_VIEWER_DIR}
# root binary directory = ${PALLET_VIEWER_DIR}

cmake_minimum_required (VERSION 2.6)
project (pallet_viewer)

SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_NAME "pallet_viewer")
SET(CPACK_SYSTEM_NAME "i386")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "glutg3 (>= 3.7), glutg3-dev (>= 3.7)")

set (CMAKE_VERBOSE_MAKEFILE OFF)
if(NOT MSVC)
  set (CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -Wpointer-arith -fno-default-inline -fno-implicit-inline-templates")
endif(NOT MSVC)

# add_subdirectory (gl)

option (PACKLIST_UNITTEST "UnitTest to parse a .packlist.xml" ON)

if (PACKLIST_UNITTEST)
    set_property( SOURCE main.cpp
       APPEND PROPERTY COMPILE_DEFINITIONS PACKLIST_UNITTEST
    )
endif (PACKLIST_UNITTEST)

set( SRC_GUI 
gl/view
)

set ( SRC_FILES
packlist
response
xml_parser 
main
)

set ( SRC 
${SRC_GUI}
${SRC_FILES}
)

set ( HEADERS
./gl
/usr/include
)

find_package(GLUT)
find_package(OpenGL)
 
set(PROGRAMS oglfirst pointers)
set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} m)
 
include_directories (${HEADERS})
add_executable (pallet_viewer ${SRC})
target_link_libraries (pallet_viewer ${CORELIBS} )

INSTALL_TARGETS(/bin pallet_viewer)
SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
SET(CPACK_PACKAGE_CONTACT "pushkar@cc.gatech.edu")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Pushkar Kolhe (${CPACK_PACKAGE_CONTACT})")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pallet Viewer and Benchmarking for the ICRA VMAC 2010 competition.")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_PACKAGE_VENDOR "VMAC 2010, NIST and GT")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")

SET(CPACK_GENERATOR "DEB")

SET(CPACK_PACKAGE_EXECUTABLES "pallet_viewer" "${CMAKE_CURRENT_SOURCE_DIR}/build/pallet_viewer")

INCLUDE(CPack)

