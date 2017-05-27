#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |  Fluicell AB - Lab-on-a-tip                                               |
#  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
#  |                                                                           |
#  | Copyright (c) 2017,                                                       |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 
#
#################################################################################################
#
# ---------      DANGEROUS CODE !!!!! to set environment variables for debugging 
# ---------      read this carefully before using this file !!! 
# ---------      http://cmake.limitpoint.com/setting-the-visual-studio-debugger-path-using-cmake/
#
#################################################################################################

SET (PATHS_TO_DLL ${QT_BINARY_DIR})
LIST(APPEND PATHS_TO_DLL ${CMAKE_MODULE_PATH}) # Directory for CMake scripts
#LIST(APPEND PATHS_TO_DLL ${OpenCV_DIR}/bin/Release) #TODO: MB - check this 
#LIST(APPEND PATHS_TO_DLL ${VTK_DIR}/bin/Debug) 
#LIST(APPEND PATHS_TO_DLL ${VTK_DIR}/bin/Release) 


# Find user and system name
SET(SYSTEM_NAME $ENV{USERDOMAIN} CACHE STRING SystemName)
SET(USER_NAME $ENV{USERNAME} CACHE STRING UserName)

# Configure the template file
SET(USER_FILE ${_projectName}.vcproj.${SYSTEM_NAME}.${USER_NAME}.user)
SET(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
#CONFIGURE_FILE(UserTemplate.user ${USER_FILE} @ONLY)
configure_file(${CMAKE_MODULE_PATH}/project.vcxproj.user.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user @ONLY) 
message (STATUS " Set environment variables activated, \n the following paths will be added to debug local environment paths: \n\n     ${PATHS_TO_DLL} \n" )

# the user may configure other environment variables changing 
#configure_file(./CMakeModules/project.vcxproj.user.in ${CMAKE_BINARY_DIR}/${PROJECT_2_NAME}.vcxproj.user @ONLY) 

# use the variable PATHS_TO_DLL to append other paths -----
# this variable is referenced in the xml file "project.vcxproj.user.in" using --> @PATHS_TO_DLL@ for relocable paths
