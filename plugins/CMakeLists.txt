#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | Fluicell AB, http://fluicell.com/                                         |
#  | Plugins                                                                   |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ */

project(ALL_Fluicell_PLUGINS)

# Enumerate all subdirs and process them:
list_subdirectories(LIST_PLUGINS_DIRS "${CMAKE_SOURCE_DIR}/plugins")

foreach(dir ${LIST_PLUGINS_DIRS})
	SET(VAR_NAME "PLUGINS_BUILD_${dir}")
	SET(${VAR_NAME} ON CACHE BOOL "Build the plugin ${dir}")
	MARK_AS_ADVANCED(${VAR_NAME})

	if (${VAR_NAME})
	add_subdirectory(${dir})
	#TODO MAURO  check for remove subdirectory in case of subdirectory not defined 
	endif (${VAR_NAME})
endforeach(dir)
