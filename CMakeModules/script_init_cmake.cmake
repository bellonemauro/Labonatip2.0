#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |  Fluicell AB - Lab-on-a-tip                                               |
#  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

#  Loads the current version number:
FILE(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version_prefix.txt" CMAKE_Fluicell_VERSION_NUMBER LIMIT_COUNT 1) # Read only the first line

# For example: "0.5.1"
STRING(REGEX MATCHALL "[0-9]+" CMAKE_Fluicell_VERSION_PARTS "${CMAKE_Fluicell_VERSION_NUMBER}")

LIST(GET CMAKE_Fluicell_VERSION_PARTS 0 CMAKE_Fluicell_VERSION_NUMBER_MAJOR)
LIST(GET CMAKE_Fluicell_VERSION_PARTS 1 CMAKE_Fluicell_VERSION_NUMBER_MINOR)
LIST(GET CMAKE_Fluicell_VERSION_PARTS 2 CMAKE_Fluicell_VERSION_NUMBER_PATCH)
SET(CMAKE_Fluicell_FULL_VERSION "${CMAKE_Fluicell_VERSION_NUMBER_MAJOR}.${CMAKE_Fluicell_VERSION_NUMBER_MINOR}.${CMAKE_Fluicell_VERSION_NUMBER_PATCH}" )

SET(SOFTWARE_VERSION ${CMAKE_Fluicell_FULL_VERSION}  CACHE STRING "Fluicell software version " FORCE)

IF(WIN32)
	SET(Fluicell_DLL_VERSION_POSTFIX "${CMAKE_Fluicell_VERSION_NUMBER_MAJOR}${CMAKE_Fluicell_VERSION_NUMBER_MINOR}${CMAKE_Fluicell_VERSION_NUMBER_PATCH}")
ELSE(WIN32)
	SET(Fluicell_DLL_VERSION_POSTFIX "")
ENDIF(WIN32)


# Setup output Directories 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin  CACHE PATH "Single Directory for all Libraries" ) 
# libraries and executables are going into the same folder to avoid annoying environmental variables in MSVC 

# Setup the Executables output Directory 
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin  CACHE PATH "Single Directory for all Executables." )

# Setup the archive output Directory 
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin  CACHE PATH "Single Directory for all static libraries.")


message(STATUS " ---------------------------------- " )
message(STATUS " Fluicell software information: " )
message(STATUS "    Software version : ${CMAKE_Fluicell_FULL_VERSION}" )
message(STATUS "    Output directory : ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" )
message(STATUS "    Website          : www.fluicell.com " )
message(STATUS " ---------------------------------- " )



#set CMAKEMODULES paths
SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}"  CACHE PATH "Single Directory for all CMake scripts" )   

#support for shared libraries
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)  


## Check for Version ##
if ( ENABLE_verbose )
  message(STATUS " ---------------------------------- " )
  message(STATUS " System information: " )
  message(STATUS "    CMAKE_VERSION          : ${CMAKE_VERSION}" )
  message(STATUS "    CMAKE_GENERATOR        : ${CMAKE_GENERATOR}" )
  message(STATUS "    CMAKE_SYSTEM           : ${CMAKE_SYSTEM}" )
  message(STATUS "    CMAKE_SYSTEM_NAME      : ${CMAKE_SYSTEM_NAME}" )
  message(STATUS "    CMAKE_SYSTEM_VERSION   : ${CMAKE_SYSTEM_VERSION}" )
  message(STATUS "    CMAKE_SYSTEM_PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}" )
  message(STATUS "    CMAKE_CL_64            : ${CMAKE_CL_64}    0/1 ==> 32/64 bit architecture")
  message(STATUS " System flags: " )
  message(STATUS "       UNIX  : ${UNIX}" )
  message(STATUS "       WIN32 : ${WIN32}" )
  message(STATUS "       APPLE : ${APPLE}" )
  message(STATUS "       MSVC  : ${MSVC}" )
  message(STATUS "       MSVC_VERSION  : ${MSVC_VERSION}" ) # check here what the answer means https://cmake.org/cmake/help/v3.0/variable/MSVC_VERSION.html
  message(STATUS " Compiler information: " )
  message(STATUS "    CMAKE_C_COMPILER_ID      : ${CMAKE_C_COMPILER_ID}" )
  message(STATUS "    CMAKE_CXX_COMPILER_ID    : ${CMAKE_CXX_COMPILER_ID}" )
  message(STATUS "    CMAKE_COMPILER_IS_GNUCC  : ${CMAKE_COMPILER_IS_GNUCC}" )
  message(STATUS "    CMAKE_COMPILER_IS_GNUCXX : ${CMAKE_COMPILER_IS_GNUCXX}" )
  message(STATUS " ---------------------------------- \n" )
endif ( ENABLE_verbose )


# find different libraries for different OSs
IF(WIN32)
   SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll)
ELSEIF(UNIX)
   SET(CMAKE_FIND_LIBRARY_SUFFIXES .a .so) #A simple unix configuration for libraries 
ENDIF()

# Set optimized building (must go BEFORE declaring the targets)
IF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mtune=native")
ENDIF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")

# Disable annoying deprecated warnings in MSVC
if(MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)   
endif(MSVC)



#check for 64 or 32 bit architecture
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	set(BITNESS 64)
else()
	set(BITNESS 32)
endif()

# A function to get all user defined variables with a specified prefix to allow groups in cmake
function (getListOfVarsStartingWith _prefix _varResult)
    get_cmake_property(_vars CACHE_VARIABLES)
    string (REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9_]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()


# ----------------------------------------------------------------------------
# Detect GNU version:
# ----------------------------------------------------------------------------
IF(CMAKE_COMPILER_IS_GNUCXX)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
		          OUTPUT_VARIABLE CMAKE_Fluicell_GCC_VERSION_FULL
		          OUTPUT_STRIP_TRAILING_WHITESPACE)

	# Output in CMAKE_Fluicell_GCC_VERSION_FULL: "X.Y"
	#  Look for the version number
	STRING(REGEX MATCH "[0-9]+.[0-9]+" CMAKE_GCC_REGEX_VERSION "${CMAKE_Fluicell_GCC_VERSION_FULL}")

	# Split the three parts:
	STRING(REGEX MATCHALL "[0-9]+" CMAKE_Fluicell_GCC_VERSIONS "${CMAKE_GCC_REGEX_VERSION}")

	LIST(GET CMAKE_Fluicell_GCC_VERSIONS 0 CMAKE_Fluicell_GCC_VERSION_MAJOR)
	LIST(GET CMAKE_Fluicell_GCC_VERSIONS 1 CMAKE_Fluicell_GCC_VERSION_MINOR)

	SET(CMAKE_CADSAMPLES_GCC_VERSION ${CMAKE_Fluicell_GCC_VERSION_MAJOR}${CMAKE_Fluicell_GCC_VERSION_MINOR})
	
	if (CMAKE_VERSION VERSION_LESS "3.1")
	    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	      set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
	    endif ()
	else ()
	    set (CMAKE_CXX_STANDARD 11)
	endif ()

	IF($ENV{VERBOSE})
		MESSAGE(STATUS "gcc -dumpversion: ${CMAKE_Fluicell_GCC_VERSION_FULL}  -> Major=${CMAKE_Fluicell_GCC_VERSION_MAJOR} Minor=${CMAKE_Fluicell_GCC_VERSION_MINOR}")
	ENDIF($ENV{VERBOSE})
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



