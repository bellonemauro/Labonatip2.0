#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |  Fluicell AB - Lab-on-a-tip                                               |
#  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

project(PPC1api_test_cmdline)

#  including external libraries
include_directories(${PPC1api_INCLUDE_DIR})

include_directories(${serial_INCLUDE_DIR})

if ( ENABLE_verbose )
	message (STATUS "${PROJECT_NAME} MESSAGE: serial_INCLUDE_DIR    :: ${serial_INCLUDE_DIR}")
	message (STATUS "${PROJECT_NAME} MESSAGE: PPC1api_INCLUDE_DIR    :: ${mylib_lib_INCLUDE_DIR}")
endif ( ENABLE_verbose )

	
#add_dependencies(${PROJECT_NAME}  PPC1api )

add_executable( ${PROJECT_NAME} PPC1api_test_cmdline.cpp )

target_link_libraries (${PROJECT_NAME}  PPC1api  serial )


# allows folders for MSVC
if (MSVC AND ENABLE_SOLUTION_FOLDERS) 
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Apps")
endif (MSVC AND ENABLE_SOLUTION_FOLDERS)
