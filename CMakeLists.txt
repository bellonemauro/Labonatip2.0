#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |  Fluicell AB - Lab-on-a-tip                                               |
#  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

project(Fluicell)   # Fluicell lab-on-a-tip

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

option(ENABLE_verbose "Verbose cmake cofiguration " "ON")
if ( ENABLE_verbose )
	message(WARNING " Verbose configuration active ! You may get a lot of messages now ... " )
endif ( ENABLE_verbose )

#cmake tweaks
if(COMMAND cmake_policy) #return true if the COMMAND cmake_policy exist - this allows compatibility with older versions
	# We don't want to mix relative and absolute paths in linker lib lists.
	cmake_policy(SET CMP0003 NEW) # Required by CMake 2.7+
	
	# avoid link warnings - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0015.html
	cmake_policy(SET CMP0015 NEW)
	
	#Ignore COMPILE_DEFINITIONS_<Config> properties - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0043.html
	cmake_policy(SET CMP0043 NEW) 
	
	#The target in the TARGET signature of add_custom_command() must exist - see https://cmake.org/cmake/help/v3.0/policy/CMP0040.html
	cmake_policy(SET CMP0040 NEW)
	
	# Allow using the LOCATION target property.
	if(NOT "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.0)
		cmake_policy(SET CMP0026 OLD)
	endif(NOT "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.0)
endif(COMMAND cmake_policy)

# Our own CMake configurations
include(${PROJECT_SOURCE_DIR}/CMakeModules/script_init_cmake.cmake REQUIRED)		

include(${CMAKE_MODULE_PATH}/UtilsMacros.cmake REQUIRED)		        # CMake macros

set (Fluicell_SOURCE_DIR ${PROJECT_SOURCE_DIR})


# find Eigen 
option(ENABLE_internalEigen "Use internal Eigen library " "ON")
If (NOT EIGEN_INCLUDE_DIR OR ENABLE_internalEigen)
  find_package(Eigen )
  message(STATUS "Using internal Eigen lib " )
  set (EIGEN_INCLUDE_DIR  ${PROJECT_SOURCE_DIR}/3rdParty/Eigen_3.2.2/Eigen  CACHE PATH "Path to the internal Eigen version" FORCE)
  include_directories(${EIGEN_INCLUDE_DIR})
  message(STATUS "     EIGEN_INCLUDE_DIR : ${EIGEN_INCLUDE_DIR} \n" )
  add_definitions(-DEIGEN_NO_DEBUG) 
  add_definitions(-DNDEBUG)
else ()
  include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})
  include_directories(${EIGEN_INCLUDE_DIR})
  add_definitions(-DEIGEN_NO_DEBUG) 
  add_definitions(-DNDEBUG)
endif()


#  Add option enable visual studio to create folders
option(ENABLE_SOLUTION_FOLDERS "Enable folders for MSVC - ON by default if MSVC - OFF otherwise" ON)
if (MSVC)
  set (ENABLE_SOLUTION_FOLDERS 1)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else (MSVC)
  set (ENABLE_SOLUTION_FOLDERS 0)
endif (MSVC) 

# Allows to set local environmental variable to debug in visual studio
option(ENABLE_setLocalDebugEnvironment "Set the local environment variables for debugging --- USE THIS ONLY FOR DEBUG" "OFF")
if(MSVC AND ENABLE_setLocalDebugEnvironment)
#this is not for all users !!!!
include(${CMAKE_MODULE_PATH}/script_setLocalDebugEnvironment.cmake REQUIRED)
# Add in this file your debug local environment configurations
endif(MSVC AND ENABLE_setLocalDebugEnvironment)


# -------------------- 3rParty --------------------------------
OPTION(ENABLE_BUILD_3rdParty "Option for building 3rdParty libraries - ON by default" ON)
if (ENABLE_BUILD_3rdParty)
  add_subdirectory(3rdParty)
endif(ENABLE_BUILD_3rdParty)

#set(BUILD_3RD_serial OFF  CACHE BOOL "" FORCE )


# add all libraries in the /libs folder
OPTION(ENABLE_BUILD_libs "Option for building internal libraries - ON by default" ON)
if (ENABLE_BUILD_libs)
	add_subdirectory(libs)
endif(ENABLE_BUILD_libs)

# add all libraries in the /plugins folder
OPTION(ENABLE_BUILD_plugins "Option for building internal plugins - ON by default" OFF)
if (ENABLE_BUILD_plugins)
	add_subdirectory(plugins)
endif(ENABLE_BUILD_plugins)

# -----------  Fluicell_libs  --------------------
#include_directories("${Fluicell_lib_INCLUDE_DIR}")   #TODO 


# -------------------- apps --------------------------------
OPTION(ENABLE_BUILD_apps "Option for building apps - ON by default" ON)
if (ENABLE_BUILD_apps)
	add_subdirectory(apps)
endif(ENABLE_BUILD_apps)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile_labonatip.in ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile @ONLY)
add_custom_target(Lab-on-a-tip_docs 
	COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile 
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM )

if(MSVC)
	set_target_properties(Lab-on-a-tip_docs PROPERTIES FOLDER "documentation")
endif(MSVC)

endif(DOXYGEN_FOUND)



# --------------------------------------
#  Copy useful resources to the build folder
# --------------------------------------					
# file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)


# --------------------------------------
#  settings for install and package --- not fully supported yet
# --------------------------------------					
include(${PROJECT_SOURCE_DIR}/CMakeModules/script_BuildInstall.cmake REQUIRED)		# CMake configurations
include(${PROJECT_SOURCE_DIR}/CMakeModules/script_BuildPackage.cmake REQUIRED)		# CMake configurations						
