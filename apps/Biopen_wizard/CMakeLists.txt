#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | Fluicell AB, http://fluicell.com/                                         |
#  | Biopen wizard                                                             |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

project(Biopen_wizard)

# --------------------------------------
#  set variables for compilation 
# --------------------------------------
# select all headers in the include dir
FILE(GLOB myProject_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")   
# select all sources in the include dir
FILE(GLOB myProject_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")  
# select all files for the graphical interface
FILE(GLOB myProject_FORMS "${PROJECT_SOURCE_DIR}/forms/*.ui")
# select all files for the resources
FILE(GLOB myProject_UI_RESOURCES "${PROJECT_SOURCE_DIR}/*.qrc")
# select all files for the resources
SET (myProject_LANGUAGES_DIR "${PROJECT_SOURCE_DIR}/languages/")
FILE(GLOB myProject_LANGUAGES_TS "${myProject_LANGUAGES_DIR}/*.ts")

SET (ENABLE_WINDEPLOYQT 0 CACHE BOOL "Set to 1 to activate deployment of QT apps, 0 by default" )

include_directories(${PPC1api_INCLUDE_DIR})
include_directories(${serial_INCLUDE_DIR})
if ( ENABLE_verbose )
	message (STATUS "${PROJECT_NAME} MESSAGE: PPC1api_INCLUDE_DIR    :: ${PPC1api_INCLUDE_DIR}")
	message (STATUS "${PROJECT_NAME} MESSAGE: serial_INCLUDE_DIR    :: ${serial_INCLUDE_DIR}")
endif ( ENABLE_verbose )


# resource file for icon
IF(WIN32)
	SET(myProject_SOURCES ${myProject_SOURCES} resources.rc)
ENDIF()
	
# this is necessary to find in the code the ui____.h file built in the build folder
include_directories(${PROJECT_BINARY_DIR}) 
include_directories(${PROJECT_SOURCE_DIR}/include/)

# copy icons and configuration files to the building folder
if(MSVC)
  FILE(GLOB myProject_SETTINGS "${PROJECT_SOURCE_DIR}/settings/*.ini") 
  FOREACH(F ${myProject_SETTINGS})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/settings/)
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}/settings/)
  ENDFOREACH(F)				
 
  FILE(GLOB myProject_EXTDATA "${PROJECT_SOURCE_DIR}/Ext_data/*.*") 
  FOREACH(F ${myProject_EXTDATA})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/Ext_data/)
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}/Ext_data/)
  ENDFOREACH(F)
  
  FILE(GLOB myProject_GUIDE "${PROJECT_SOURCE_DIR}/guide/*.*") 
  FOREACH(F ${myProject_GUIDE})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/guide/)
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}/guide/)
  ENDFOREACH(F)
  
  FILE(GLOB myProject_PRESETPROTOCOLS "${PROJECT_SOURCE_DIR}/presetProtocols/*.*") 
  FOREACH(F ${myProject_PRESETPROTOCOLS})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/presetProtocols/)
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}/presetProtocols/)
  ENDFOREACH(F)

#  FILE(GLOB myProject_LANGUAGES "${PROJECT_SOURCE_DIR}/languages/*.qm") 
#  FOREACH(F ${myProject_LANGUAGES})
#	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/Biopen/languages/)
#	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/Biopen/languages/)
#  ENDFOREACH(F)
  
elseif( NOT MSVC)
  FILE(GLOB myProject_SETTINGS "${PROJECT_SOURCE_DIR}/settings/*.ini") 
  FOREACH(F ${myProject_SETTINGS})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_NAME}/settings/)
  ENDFOREACH(F)				

  FILE(GLOB myProject_EXTDATA "${PROJECT_SOURCE_DIR}/Ext_data/*.*") 
  FOREACH(F ${myProject_EXTDATA})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_NAME}/Ext_data/)
  ENDFOREACH(F)
  
    FILE(GLOB myProject_GUIDE "${PROJECT_SOURCE_DIR}/guide/*.*") 
  FOREACH(F ${myProject_GUIDE})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_NAME}/guide/)
  ENDFOREACH(F)
  
  FILE(GLOB myProject_PRESETPROTOCOLS "${PROJECT_SOURCE_DIR}/presetProtocols/*.*") 
  FOREACH(F ${myProject_PRESETPROTOCOLS})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_NAME}/presetProtocols/)
  ENDFOREACH(F)
  
#  FILE(GLOB myProject_LANGUAGES "${PROJECT_SOURCE_DIR}/languages/*.qm") 
#  FOREACH(F ${myProject_LANGUAGES})
#	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Biopen/languages/)
#  ENDFOREACH(F)

  endif()







if(with_QT6)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set (QMAKE_CFLAGS_UTF8_SOURCE "/source-charset:windows-1252")
	
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOUIC_SEARCH_PATHS "./forms/")

	find_package(Qt6 COMPONENTS Core Core5Compat Widgets Network Gui Concurrent LinguistTools Charts TextToSpeech REQUIRED) 

	
	# --------------------------------------
	#  add definitions
	# --------------------------------------
	add_definitions (${QT_DEFINITIONS})   #required to use QT and gui
	add_definitions (-DLABONATIP_VERSION=${SOFTWARE_VERSION})   #required to use QT and gui
	add_definitions (-DUSEQT6)
	
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
							
	#target_link_libraries ( ${PROJECT_NAME}  ${QT_LIBRARIES} ${Qt5SerialPort_LIBRARIES})
	target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Core5Compat Qt6::Widgets Qt6::Network Qt6::Gui Qt6::Concurrent  Qt6::Charts PPC1api serial Qt6::TextToSpeech) 
	
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
		#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> COMMENT "Preparing Qt runtime dependencies")
		
		
		# this is done to copy dll from the "iCLS client" which are required in the deployment with QNetwork for the communication
		# they are different for 64bit and 32bit
		if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
			set(iCLS_Client_x32_DIR "$ENV{ProgramFiles} (x86)/Intel/iCLS Client/")
			message (STATUS "${PROJECT_NAME} MESSAGE: iCLS_Client_x32_DIR  :: ${iCLS_Client_x32_DIR} ")	
			FILE(GLOB iCLS_Client_x32_dll "${iCLS_Client_x32_DIR}/*32.dll") 
			
			#TODO: here there is a static link to the program file folder 
			#      with the "Intel/iCLS client" but there is no check for existence
			list( LENGTH iCLS_Client_x32_dll dll_num )
			if (dll_num EQUAL 0)
				message (WARNING "${PROJECT_NAME} MESSAGE: no dll found in ${iCLS_Client_x32_DIR} this may cause problems in ssl connection to updates")	
			endif()
			 
			 FOREACH(F ${iCLS_Client_x32_dll})
				file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/)
				message (STATUS "            ${PROJECT_NAME} MESSAGE: copied file :: ${F} to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/")
			  ENDFOREACH(F)					
			
		elseif("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")			
			set(iCLS_Client_x64_DIR "$ENV{PROGRAMFILES}/Intel/iCLS Client/")
			message (STATUS "${PROJECT_NAME} MESSAGE: iCLS_Client_x64_DIR  :: ${iCLS_Client_x64_DIR} ")
			FILE(GLOB iCLS_Client_x64_dll "${iCLS_Client_x64_DIR}/*32.dll") 				
			
			#TODO: here there is a static link to the program file folder 
			#      with the "Intel/iCLS client" but there is no check for existence
			list( LENGTH iCLS_Client_x64_dll dll_num )
			if (dll_num EQUAL 0)
				message (WARNING "${PROJECT_NAME} MESSAGE: no dll found in ${iCLS_Client_x64_DIR} this may cause problems in ssl connection to updates")	
			endif()
			
			FOREACH(F ${iCLS_Client_x64_dll})
				file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/)
				message (STATUS "            ${PROJECT_NAME} MESSAGE: copied file :: ${F} to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/")
			ENDFOREACH(F)				

		endif()	
	endif()
else (with_QT6)

	# --------------------------------------
	#  add definitions
	# --------------------------------------
	add_definitions (${QT_DEFINITIONS})   #required to use QT and gui
	add_definitions (-DLABONATIP_VERSION=${SOFTWARE_VERSION})   #required to use QT and gui

	# Add options to build specific targets
	# allows automatic linking of QT - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0020.html
	cmake_policy(SET CMP0020 NEW)

	find_package(Qt5Core 5.14 REQUIRED) 

	# Find the QtWidgets library 
	find_package(Qt5 COMPONENTS Core Widgets Network Gui Charts Concurrent LinguistTools TextToSpeech REQUIRED)
	# set Qt5DIR to ./Qt/__VERSION__/__COMPILER__/lib/cmake/Qt5

	# set the policy for the automoc generation, only for cmake >3.10
	# see https://cmake.org/cmake/help/git-stage/policy/CMP0071.html
	if(${CMAKE_VERSION} VERSION_GREATER "3.10.0") 
		cmake_policy(SET CMP0071 NEW)
	endif()

	#TODO: this is fine for now but it is not the best solution, 
	#      this _qt_bin_dir should be the right variable for binaries
	SET (QT5_BINARY_DIR ${_qt5_install_prefix}/../../bin/ CACHE PATH "Qt5 binary folder" )
		
	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: _qt5_install_prefix ---- ${_qt5_install_prefix}")
		message (STATUS "${PROJECT_NAME} MESSAGE: _qt5_bin_dir ---- ${_qt5_bin_dir}")	
		message (STATUS "${PROJECT_NAME} MESSAGE: QT5_BINARY_DIR ---- ${QT5_BINARY_DIR}")
	endif ( ENABLE_verbose )

	if (NOT QT5_BINARY_DIR)
		message(WARNING "${PROJECT_NAME} MESSAGE: QT5_BINARY_DIR not found --- set manually")
	endif()


	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	# allows the gui to be automatically compiled and generate the ui_* headers 
	QT5_WRAP_UI(myProject_FORMS_HEADERS ${myProject_FORMS})

	#this allows the executable to encode all the icons 
	QT5_ADD_RESOURCES(UI_RESOURCES_RCC ${myProject_UI_RESOURCES})


	option(BUILD_TRANSLATION "Build translation from ts files" "OFF")
	if (BUILD_TRANSLATION)
		# this allows the automatic translation from cmake  
		# step1:
		# set the output folder we have to run this command, 
		# see https://stackoverflow.com/questions/44782914/cmake-qt5-add-translation-how-to-specify-the-output-path
		set_source_files_properties(${myProject_LANGUAGES_TS} PROPERTIES OUTPUT_LOCATION ${myProject_LANGUAGES_DIR})
		
		#TODO: language files got deleted after clean-build in MSVC !!!! why?
		
		# step2: 
		# create translation files
		#qt5_create_translation(QT_TRANSLATIONS_FILES ${myProject_LANGUAGES_DIR} ${myProject_LANGUAGES_TS})
		#TODO: when qt5_create_translation if active, visual studio clean-build delete automatically TS files
		#      this is not possible to happen
		SET (TRANSLATION_GENERATOR "${QT5_BINARY_DIR}/lrelease.exe")
		if ( ENABLE_verbose )
			message (STATUS "${PROJECT_NAME} MESSAGE: myProject_LANGUAGES_TS  :: ${myProject_LANGUAGES_TS} ")
			message (STATUS "${PROJECT_NAME} MESSAGE: myProject_LANGUAGES_DIR  :: ${myProject_LANGUAGES_DIR} ")
			message (STATUS "${PROJECT_NAME} MESSAGE: QT_TRANSLATIONS_FILES  :: ${QT_TRANSLATIONS_FILES} ")	
			message (STATUS "${TRANSLATION_GENERATOR} MESSAGE: TRANSLATION_GENERATOR  :: ${TRANSLATION_GENERATOR} ")
		endif ( ENABLE_verbose )

		# step 3 
		# run the command  " lrelease.exe _file.ts _file.qm " for each ts file 
		# TODO: this is to be checked in linux
		if(WIN32)
		set (idx 0)
		  FOREACH(F1 ${myProject_LANGUAGES_TS})
			list (GET QT_TRANSLATIONS_FILES ${idx} F2)
			execute_process(COMMAND ${TRANSLATION_GENERATOR} ${F1} ${F2})
			
			if ( ENABLE_verbose )
				message (STATUS " Added custom command ::: ${TRANSLATION_GENERATOR} ${F1} ${F2} ")
			endif ( ENABLE_verbose )	  
			MATH(EXPR idx "${idx}+1")
		  ENDFOREACH(F1)	
		endif()

	endif (BUILD_TRANSLATION)

	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: myProject_FORMS_HEADERS  :: ${myProject_FORMS_HEADERS} ")
	endif ( ENABLE_verbose )

	add_executable (${PROJECT_NAME}  	${myProject_SOURCES} ${myProject_HEADERS}
									${myProject_FORMS_HEADERS} ${UI_RESOURCES_RCC})
	qt5_use_modules(${PROJECT_NAME} Network Widgets TextToSpeech)

	if (WIN32)
	set_target_properties (${PROJECT_NAME} 
							PROPERTIES 
							ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${PROJECT_NAME}"
							ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}"
							LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}"
							RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}")
	endif()
	if (UNIX)
		set_target_properties (${PROJECT_NAME}
														PROPERTIES
														ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${PROJECT_NAME}"
														LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PROJECT_NAME}"
														RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}")

	endif()


	target_link_libraries ( ${PROJECT_NAME}  
							${QT_LIBRARIES}  ${Qt5Charts_LIBRARIES} 
							PPC1api serial )
						
	if (VLD_MemoryCheck)
		message (STATUS "${PROJECT_NAME} Memory leak detector activated ")
		target_link_libraries (${PROJECT_NAME} 	${VLD_LIBRARY_VLD})
	endif ( )

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
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> --no-translations COMMENT "Preparing Qt runtime dependencies")
		#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> COMMENT "Preparing Qt runtime dependencies")
		
		
		# this is done to copy dll from the "iCLS client" which are required in the deployment with QNetwork for the communication
		# they are different for 64bit and 32bit
		if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
			set(iCLS_Client_x32_DIR "$ENV{ProgramFiles} (x86)/Intel/iCLS Client/")
			message (STATUS "${PROJECT_NAME} MESSAGE: iCLS_Client_x32_DIR  :: ${iCLS_Client_x32_DIR} ")	
			FILE(GLOB iCLS_Client_x32_dll "${iCLS_Client_x32_DIR}/*32.dll") 
			
			#TODO: here there is a static link to the program file folder 
			#      with the "Intel/iCLS client" but there is no check for existence
			list( LENGTH iCLS_Client_x32_dll dll_num )
			if (dll_num EQUAL 0)
				message (WARNING "${PROJECT_NAME} MESSAGE: no dll found in ${iCLS_Client_x32_DIR} this may cause problems in ssl connection to updates")	
			endif()
			 
			 FOREACH(F ${iCLS_Client_x32_dll})
				file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/)
				message (STATUS "            ${PROJECT_NAME} MESSAGE: copied file :: ${F} to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/")
			  ENDFOREACH(F)					
			
		elseif("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")			
			set(iCLS_Client_x64_DIR "$ENV{PROGRAMFILES}/Intel/iCLS Client/")
			message (STATUS "${PROJECT_NAME} MESSAGE: iCLS_Client_x64_DIR  :: ${iCLS_Client_x64_DIR} ")
			FILE(GLOB iCLS_Client_x64_dll "${iCLS_Client_x64_DIR}/*32.dll") 				
			
			#TODO: here there is a static link to the program file folder 
			#      with the "Intel/iCLS client" but there is no check for existence
			list( LENGTH iCLS_Client_x64_dll dll_num )
			if (dll_num EQUAL 0)
				message (WARNING "${PROJECT_NAME} MESSAGE: no dll found in ${iCLS_Client_x64_DIR} this may cause problems in ssl connection to updates")	
			endif()
			
			FOREACH(F ${iCLS_Client_x64_dll})
				file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/)
				message (STATUS "            ${PROJECT_NAME} MESSAGE: copied file :: ${F} to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PROJECT_NAME}/")
			ENDFOREACH(F)				

		endif()	
	endif()
endif()
# allows folders for MSVC
if (MSVC AND ENABLE_SOLUTION_FOLDERS) 
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Apps")
endif (MSVC AND ENABLE_SOLUTION_FOLDERS)



