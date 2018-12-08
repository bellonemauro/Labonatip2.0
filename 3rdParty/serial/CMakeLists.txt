#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | Fluicell AB, http://fluicell.com/                                         |
#  | Light Serial communication library                                        |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ */
 
# Note: This CMake file is not standalone, it defines the serial 3rd party library 
project(serial) 
 
 
set (lib_SOURCE_DIR ${serial_SOURCE_DIR} ) 
 
#select all headers in the include dir 
#FILE(GLOB 3RD_HEADERS "${lib_SOURCE_DIR}/include/*.h")    
#select all sources in the include dir 
#FILE(GLOB 3RD_SOURCES "${lib_SOURCE_DIR}/src/*.cpp")   
 
 
## Sources 
set(serial_SRCS 
    src/serial.cc 
    include/serial/serial.h 
    include/serial/v8stdint.h 
) 
 

if(APPLE) 
        # If OSX 
        list(APPEND serial_SRCS src/impl/unix.cc) 
        list(APPEND serial_SRCS src/impl/list_ports/list_ports_osx.cc) 
elseif(UNIX) 
    # If unix 
    list(APPEND serial_SRCS src/impl/unix.cc) 
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_linux.cc) 
else() 
    # If windows 
    list(APPEND serial_SRCS src/impl/win.cc) 
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_win.cc) 
endif() 
 
 
## Add serial library 
add_library(${PROJECT_NAME} ${serial_SRCS}) 
 
# set the postfix _d for debug libraries  
set_target_properties(${PROJECT_NAME}  PROPERTIES DEBUG_POSTFIX "_d") 
set (serial_INCLUDE_DIR ${lib_SOURCE_DIR}/include CACHE PATH "Path to the headers of serial lib") 
 
if(APPLE) 
        target_link_libraries(${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY}) 
elseif(UNIX) 
        target_link_libraries(${PROJECT_NAME} rt pthread) 
else() 
        target_link_libraries(${PROJECT_NAME} setupapi) 
endif() 
 
## Include headers 
include_directories(include) 
 
# allows folders for MSVC 
if (MSVC AND ENABLE_SOLUTION_FOLDERS)  
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "3rdParty") 
endif (MSVC AND ENABLE_SOLUTION_FOLDERS)