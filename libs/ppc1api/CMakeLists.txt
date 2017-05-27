#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |  Fluicell AB - Lab-on-a-tip                                               |
#  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

# Note: This CMake file is not standalone, it defines the library Nib tracking lib

project(PPC1api)   #define the project 

set (LIB_SOURCES ${PROJECT_SOURCE_DIR} )
set (LIB_HEADERS ${PROJECT_SOURCE_DIR}/include)
message (STATUS "${PROJECT_NAME} MESSAGE:   ---   LIB_SOURCES    :: ${LIB_SOURCES}")
message (STATUS "${PROJECT_NAME} MESSAGE:   ---   LIB_HEADERS    :: ${LIB_HEADERS}")

#Allows other components to find this library
set (PPC1api_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include  CACHE PATH "Path to the headers of PPC1 api") 
# necessary to allow the *.cpp to find headers 
include_directories(${PPC1api_INCLUDE_DIR})

#select all headers in the include dir
FILE(GLOB LIB_HEADERS "${LIB_SOURCES}/include/fluicell/ppc1api/*.h")   
#select all sources in the include dir
FILE(GLOB LIB_SOURCES "${LIB_SOURCES}/src/*.cpp")  

include_directories(${serial_INCLUDE_DIR})
message (STATUS "${PROJECT_NAME} MESSAGE:  serial_INCLUDE_DIR    :: ${serial_INCLUDE_DIR}")



# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DPPC1API) 

# add library 
# add_library(${PROJECT_NAME}  SHARED ${LIB_SOURCES} ${LIB_HEADERS} )
# for some reason SHARED gives a link error in ubuntu related to serial library
# Hence ---- use different modality for different OSs
# TODO: SOLVE THIS PROBLEM TO AVOID THIS WORKAROUND
IF(WIN32)
   add_library(${PROJECT_NAME}  SHARED ${LIB_SOURCES} ${LIB_HEADERS} )
ELSEIF(UNIX)
   add_library(${PROJECT_NAME}  STATIC ${LIB_SOURCES} ${LIB_HEADERS} )
ENDIF()


# set the postfix _d for debug libraries 
set_target_properties(${PROJECT_NAME}  PROPERTIES DEBUG_POSTFIX "_d")

add_dependencies(${PROJECT_NAME} serial)
target_link_libraries ( ${PROJECT_NAME}  serial)


# allows folders for MSVC
if (MSVC AND ENABLE_SOLUTION_FOLDERS) 
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libraries")
endif (MSVC AND ENABLE_SOLUTION_FOLDERS)
