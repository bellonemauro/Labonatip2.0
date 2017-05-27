#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |                      http://www.maurobellone.com                          |
#  |                                                                           |
#  | Copyright (c) 2017, - All rights reserved.                                |
#  | Authors: Mauro Bellone                                                    |
#  | Released under ___ License.                                               |
#  +---------------------------------------------------------------------------+ 

# ----------------------------------------------------------------------------
# CMake file. Run with "cmake ." at the root directory.
# ----------------------------------------------------------------------------
project(ALL_Fluicell_3RDPARTY)

# Enumerate all subdirs and process them:
list_subdirectories(LIST_3RD_DIRS "${CMAKE_SOURCE_DIR}/3rdParty")

foreach(dir ${LIST_3RD_DIRS})
	SET(VAR_NAME "BUILD_3RD_${dir}")
	SET(${VAR_NAME} ON CACHE BOOL "Build the 3rd party ${dir}")
	MARK_AS_ADVANCED(${VAR_NAME})

	if (${VAR_NAME})
	add_subdirectory(${dir})
	endif (${VAR_NAME})
endforeach(dir)

