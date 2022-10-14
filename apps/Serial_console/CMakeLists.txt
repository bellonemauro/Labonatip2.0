#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | Fluicell AB, http://fluicell.com/                                         |
#  | Biopen wizard - console app                                               |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

project(Serial_console)

# --------------------------------------
#  set variables for compilation 
# --------------------------------------
# select all headers in the include dir
FILE(GLOB myProject_HEADERS "${PROJECT_SOURCE_DIR}/*.h")   
# select all sources in the include dir
FILE(GLOB myProject_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")  
# select all files for the graphical interface
FILE(GLOB myProject_FORMS "${PROJECT_SOURCE_DIR}/*.ui")  
# select all files for the resources
FILE(GLOB myProject_UI_RESOURCES "${PROJECT_SOURCE_DIR}/*.qrc")

SET (ENABLE_WINDEPLOYQT 0 CACHE BOOL "Set to 1 to activate deployment of QT apps, 0 by default" )

# resource file for icon
IF(WIN32)
	SET(myProject_SOURCES ${myProject_SOURCES} resources.rc)
ENDIF()
	
# this is necessary to find in the code the ui____.h file built in the build folder
include_directories(${PROJECT_BINARY_DIR}) 

if(with_QT6)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set (QMAKE_CFLAGS_UTF8_SOURCE "/source-charset:windows-1252")
	
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)
    
	find_package(Qt6 COMPONENTS Core Widgets Gui  SerialPort REQUIRED) # here some package is missing, serialport charts and networking at least

	add_definitions (${QT_DEFINITIONS})   #required to use QT and gui

	add_executable (${PROJECT_NAME}  	${myProject_SOURCES} ${myProject_HEADERS}
									${myProject_FORMS_HEADERS} ${myProject_UI_RESOURCES})
	
	set_target_properties (${PROJECT_NAME} 
							PROPERTIES 
							ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}"
							LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}"
							RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}")
							
	target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets Qt6::Gui Qt6::SerialPort)
	
	# differently than others, QT has the windeployqt file that takes care of dlls and other stuff
	# see this post: http://stackoverflow.com/questions/36981942/how-to-use-add-custom-command-to-copy-dlls-using-generator-expressions
	# Windows specific build steps
	if(ENABLE_WINDEPLOYQT AND WIN32)
		# Run winddeployqt if it can be found
	find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT6_BINARY_DIR} ENV QTDIR PATH_SUFFIXES bin)
	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: WINDEPLOYQT_EXECUTABLE is : ${WINDEPLOYQT_EXECUTABLE} \n")
		message (STATUS "${PROJECT_NAME} MESSAGE: Current file target is : ${F} \n")
	endif ( ENABLE_verbose )
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME} COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> --no-translations COMMENT "Preparing Qt runtime dependencies")
	endif()
	
else(with_QT6)
	# Add options to build specific targets
	# allows automatic linking of QT - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0020.html
	cmake_policy(SET CMP0020 NEW)
	find_package(Qt5Core 5.14 REQUIRED) 
	find_package(Qt5 COMPONENTS Core Widgets Gui SerialPort REQUIRED)

	# set the policy for the automoc generation, only for cmake >3.10
	# see https://cmake.org/cmake/help/git-stage/policy/CMP0071.html
	if(${CMAKE_VERSION} VERSION_GREATER "3.10.0") 
		cmake_policy(SET CMP0071 NEW)
	endif()
	SET (QT5_BINARY_DIR ${_qt5_install_prefix}/../../bin/ CACHE PATH "Qt5 binary folder" )
		
	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: QT5_BINARY_DIR ---- ${QT5_BINARY_DIR}") #D:\Qt\5.10.0\msvc2015_64\lib\cmake\Qt5
	endif ( ENABLE_verbose )

	if (NOT QT5_BINARY_DIR)
		message(WARNING "${PROJECT_NAME} MESSAGE: QT5_BINARY_DIR not found --- set namually") #D:/Qt/5.10.0/msvc2015_64/bin
	endif()


	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	QT5_WRAP_UI(myProject_FORMS_HEADERS ${myProject_FORMS})

	#this allows the executable to encode all the icons 
	QT5_ADD_RESOURCES(UI_RESOURCES_RCC ${myProject_UI_RESOURCES})

	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: myProject_FORMS_HEADERS  :: ${myProject_FORMS_HEADERS} ")
	endif ( ENABLE_verbose )

	add_definitions (${QT_DEFINITIONS})   #required to use QT and gui

	add_executable (${PROJECT_NAME}  	${myProject_SOURCES} ${myProject_HEADERS}
									${myProject_FORMS_HEADERS} ${UI_RESOURCES_RCC})
	qt5_use_modules(${PROJECT_NAME} Widgets)

	set_target_properties (${PROJECT_NAME} 
							PROPERTIES 
							ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}"
							LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}"
							RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}")
							
	target_link_libraries ( ${PROJECT_NAME}  ${QT_LIBRARIES} ${Qt5SerialPort_LIBRARIES})



	# differently than others, QT has the windeployqt file that takes care of dlls and other stuff
	# see this post: http://stackoverflow.com/questions/36981942/how-to-use-add-custom-command-to-copy-dlls-using-generator-expressions
	# Windows specific build steps
	if(ENABLE_WINDEPLOYQT AND WIN32)
		# Run winddeployqt if it can be found
	find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT5_BINARY_DIR} ENV QTDIR PATH_SUFFIXES bin)
	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: WINDEPLOYQT_EXECUTABLE is : ${WINDEPLOYQT_EXECUTABLE} \n")
		message (STATUS "${PROJECT_NAME} MESSAGE: Current file target is : ${F} \n")
	endif ( ENABLE_verbose )
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME} COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> --no-translations COMMENT "Preparing Qt runtime dependencies")
	endif()
endif()

# allows folders for MSVC
if (MSVC AND ENABLE_SOLUTION_FOLDERS) 
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Apps")
endif (MSVC AND ENABLE_SOLUTION_FOLDERS)



