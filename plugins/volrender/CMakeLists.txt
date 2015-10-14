cmake_minimum_required(VERSION 2.8.7)

project(volrender)

# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH ${volrender_SOURCE_DIR}/CMake)

# this is EXTREMELY important to put these setting before add_library/executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set( Srcs volrender.cpp)

add_executable(volrender ${Srcs} )
  target_link_libraries(volrender 
		${VTK_LIBRARIES})
		# bnDTICommon bnHARDICommon bnDWICommon bnFiber bnVisualization bnUtility 
		# cxcore colortable vtkDICOM 

	











