#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | Fluicell AB, http://fluicell.com/                                         |
#  | Biopen wizard                                                             |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ */
#
# install internal executables file (only from the release folder)
# ONLY windows is currently supported !!!

if (WIN32 AND NOT UNIX)

	# ---- install redistributables 
	#this is not included in my build system but strangely required in the case of parallel computing installed by default on other systems
	#if (OPENMP_FOUND)   #ATTENTION !!!! this call to vcomp110.dll cannot stay here !!!! ATTENTION !!!!
	#install(FILES "C:/Program Files (x86)/Microsoft Visual Studio 11.0/VC/redist/x64/Microsoft.VC110.OpenMP/vcomp110.dll" DESTINATION ./ )#COMPONENT Libraries) #TODO specify the components
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP  ON)  # this allows the InstallRequiredSystemLibraries to find all the libraries without installing in the ./bin folder  .... so we can set the destination for the redistributables 
	include (InstallRequiredSystemLibraries )
	  FOREACH(F ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
		INSTALL(FILES "${F}" DESTINATION ./Biopen_wizard/)
		INSTALL(FILES "${F}" DESTINATION ./Serial_console/)
		INSTALL(FILES "${F}" DESTINATION ./Translator_GUI/)
	  ENDFOREACH(F)
	#install(FILES "C:/Program Files (x86)/Microsoft Visual Studio 11.0/VC/redist/x64/Microsoft.VC110.OpenMP/Microsoft.VC90.OpenMP.manifest" DESTINATION bin COMPONENT Libraries)
	#endif(OPENMP_FOUND)


	if ( ENABLE_verbose )
	  message(STATUS "\n ---------------------------------- " )
	  message(STATUS " Install script messages :  " )
	endif (  )

	if(with_QT6)
		SET(QT_BINARY_DIR ${QT6_BINARY_DIR})
	else(with_QT6)
		SET(QT_BINARY_DIR ${QT5_BINARY_DIR})
	endif()


	# EXTRACT_DEB_REL_DLLS looks for ALL .dlls in the specified library, 
	# it's of for installers but maybe allow the choice of necessary dlls only is mandatory for the sake of space saving
	EXTRACT_DEB_REL_DLLS (${QT_BINARY_DIR} d )# --> QT5 changed the postfix ! 
	if (ENABLE_verbose)
		message (STATUS "     QT_BINARY_DIR is : ${QT_BINARY_DIR}") 
		message (STATUS "\n\n REL DLLS are     : ${REL_DLLS}")
		message (STATUS "\n\n DEB DLLS are     : ${DEB_DLLS}")
	endif (ENABLE_verbose)
	  

	# differently than others, QT has the windeployqt file that takes care of dlls and other stuff
	# see this post: http://stackoverflow.com/questions/36981942/how-to-use-add-custom-command-to-copy-dlls-using-generator-expressions
	# Windows specific build steps
	if(BUILD_WINDEPLOYQT AND WIN32)
			# Run winddeployqt if it can be found
		find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT_BINARY_DIR} ENV QTDIR PATH_SUFFIXES bin)
		FILE(GLOB FILE_EXE "${PROJECT_BINARY_DIR}/bin/Release/*.exe")
		  FOREACH(F ${FILE_EXE})
			#INSTALL(FILES "${F}" DESTINATION ./)
			message (STATUS "     WINDEPLOYQT_EXECUTABLE is : ${WINDEPLOYQT_EXECUTABLE} \n")
			message (STATUS "     Current file target is : ${F} \n")
			message (STATUS "     PROJECT_NAME is : ${PROJECT_NAME} \n")
			#add_custom_command(TARGET ${F} PRE_BUILD WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin/Release/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${F}> COMMENT "Preparing Qt runtime dependencies")

		  ENDFOREACH(F)
	endif()

    INSTALL (DIRECTORY "${PROJECT_BINARY_DIR}/bin/Release/" DESTINATION ./)
    					
	if ( ENABLE_verbose )
	  message(STATUS " ---------------------------------- " )
	endif (  )

else (WIN32 AND NOT UNIX)
  message(STATUS " INSTALL NOT YET SUPPORTED " )
endif()