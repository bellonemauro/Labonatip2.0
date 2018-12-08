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
		INSTALL(FILES "${F}" DESTINATION ./)
	  ENDFOREACH(F)
	#install(FILES "C:/Program Files (x86)/Microsoft Visual Studio 11.0/VC/redist/x64/Microsoft.VC110.OpenMP/Microsoft.VC90.OpenMP.manifest" DESTINATION bin COMPONENT Libraries)
	#endif(OPENMP_FOUND)


	if ( ENABLE_verbose )
	  message(STATUS "\n ---------------------------------- " )
	  message(STATUS " Install script messages :  " )
	endif (  )

	if (QT5_BINARY_DIR AND NOT BUILD_QT5) #deprecated - QT5 is the only way to go! 
	# EXTRACT_DEB_REL_DLLS looks for ALL .dlls in the specified library, 
	# it's of for installers but maybe allow the choice of necessary dlls only is mandatory for the sake of space saving

	  message (FATAL_ERROR "  QT4 is deprecated, please use QT5 instead ")

	  EXTRACT_DEB_REL_DLLS (${QT5_BINARY_DIR} d4 )# --> OUR custom macro for searching dlls 

	  if (ENABLE_verbose)
		message (STATUS "     QT5_BINARY_DIR is : ${QT5_BINARY_DIR}")
		message (STATUS "     REL DLLS are : ${REL_DLLS}")
		message (STATUS "     DEB DLLS are : ${DEB_DLLS}")
	  endif (ENABLE_verbose)

	  FOREACH(F ${REL_DLLS})
		INSTALL(FILES "${F}" DESTINATION ./)   # install all release dll
	  ENDFOREACH(F)

	  #FOREACH(F ${DEB_DLLS})
		#INSTALL(FILES "${F}" DESTINATION bin)   # no install debug for now
	  #ENDFOREACH(F)
		
	else(QT5_BINARY_DIR AND NOT BUILD_QT5)

	# EXTRACT_DEB_REL_DLLS looks for ALL .dlls in the specified library, 
	# it's of for installers but maybe allow the choice of necessary dlls only is mandatory for the sake of space saving
	  EXTRACT_DEB_REL_DLLS (${QT5_BINARY_DIR} d )# --> QT5 changed the postfix ! 
	  if (ENABLE_verbose)
		message (STATUS "     QT5_BINARY_DIR is : ${QT5_BINARY_DIR}") 
		message (STATUS "\n\n REL DLLS are     : ${REL_DLLS}")
		message (STATUS "\n\n DEB DLLS are     : ${DEB_DLLS}")
	  endif (ENABLE_verbose)
	  

	  # differently than others, QT has the windeployqt file that takes care of dlls and other stuff
	  # see this post: http://stackoverflow.com/questions/36981942/how-to-use-add-custom-command-to-copy-dlls-using-generator-expressions
	  # Windows specific build steps
	if(BUILD_WINDEPLOYQT AND WIN32)
			# Run winddeployqt if it can be found
		find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT5_BINARY_DIR} ENV QTDIR PATH_SUFFIXES bin)
		FILE(GLOB FILE_EXE "${PROJECT_BINARY_DIR}/bin/Release/*.exe")
		  FOREACH(F ${FILE_EXE})
			#INSTALL(FILES "${F}" DESTINATION ./)
			message (STATUS "     WINDEPLOYQT_EXECUTABLE is : ${WINDEPLOYQT_EXECUTABLE} \n")
			message (STATUS "     Current file target is : ${F} \n")
			message (STATUS "     PROJECT_NAME is : ${PROJECT_NAME} \n")
			#add_custom_command(TARGET ${F} PRE_BUILD WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin/Release/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${F}> COMMENT "Preparing Qt runtime dependencies")

		  ENDFOREACH(F)
	  endif()
	  
	  
	  #FOREACH(F ${REL_DLLS})
	  #  INSTALL(FILES "${F}" DESTINATION ./)   # install all release dll
	  #ENDFOREACH(F)

	  #FOREACH(F ${DEB_DLLS})
		#INSTALL(FILES "${F}" DESTINATION ./bin)   # no install debug for now
	  #ENDFOREACH(F)
	endif ()


	# copy a specific file to the install folder
	# INSTALL(FILES "${PROJECT_SOURCE_DIR}/apps/nomeFile.extension" DESTINATION ./)		
	FILE(GLOB FILE_EXE "${PROJECT_BINARY_DIR}/bin/Release/*.*")
	  FOREACH(F ${FILE_EXE})
		INSTALL(FILES "${F}" DESTINATION ./)
	  ENDFOREACH(F)

#	FILE(GLOB INTERNAL_DLL "${PROJECT_BINARY_DIR}/bin/Release/*.dll")
#	  FOREACH(F ${INTERNAL_DLL})
#		INSTALL(FILES "${F}" DESTINATION ./)
#	  ENDFOREACH(F)

#	FILE(GLOB INTERNAL_LIB "${PROJECT_BINARY_DIR}/bin/Release/*.lib")
#	  FOREACH(F ${INTERNAL_LIB})
#		INSTALL(FILES "${F}" DESTINATION ./)
#	  ENDFOREACH(F)

	  
	  
	FILE(GLOB myProject_ICONENGINES "${PROJECT_BINARY_DIR}/bin/Release/iconengines/*.*") 
	if ( ENABLE_verbose )
		message (STATUS "     myProject_ICONENGINES : ${myProject_ICONENGINES} \n")
	endif (  )
	FOREACH(F ${myProject_ICONENGINES})
		INSTALL(FILES "${F}" DESTINATION ./iconengines)
	if ( ENABLE_verbose )
		message (STATUS "     Current file target is : ${F} \n")
	endif (  )
	ENDFOREACH(F)				

	FILE(GLOB myProject_BEARER "${PROJECT_BINARY_DIR}/bin/Release/bearer/*.*") 
	FOREACH(F ${myProject_BEARER})
		INSTALL(FILES "${F}" DESTINATION ./bearer)
	ENDFOREACH(F)
	
	FILE(GLOB myProject_GUIDE "${PROJECT_BINARY_DIR}/bin/Release/guide/*.*") 
	FOREACH(F ${myProject_GUIDE})
		INSTALL(FILES "${F}" DESTINATION ./guide)
	ENDFOREACH(F)				

	FILE(GLOB myProject_IMAGEFORMAT "${PROJECT_BINARY_DIR}/bin/Release/imageformats/*.*") 
	FOREACH(F ${myProject_IMAGEFORMAT})
		INSTALL(FILES "${F}" DESTINATION ./imageformats)
	ENDFOREACH(F)				

	FILE(GLOB myProject_PLATFORMS "${PROJECT_BINARY_DIR}/bin/Release/platforms/*.*") 
	FOREACH(F ${myProject_PLATFORMS})
		INSTALL(FILES "${F}" DESTINATION ./platforms)
	ENDFOREACH(F)				

	FILE(GLOB myProject_EXTDATA "${PROJECT_BINARY_DIR}/bin/Release/Ext_data/*.*") 
	FOREACH(F ${myProject_EXTDATA})
		INSTALL(FILES "${F}" DESTINATION ./Ext_data)
	ENDFOREACH(F)				

	FILE(GLOB myProject_MACROS "${PROJECT_BINARY_DIR}/bin/Release/presetProtocols/*.*") 
	FOREACH(F ${myProject_MACROS})
		INSTALL(FILES "${F}" DESTINATION ./presetProtocols)
	ENDFOREACH(F)		

	FILE(GLOB myProject_STYLES "${PROJECT_BINARY_DIR}/bin/Release/styles/*.*") 
	FOREACH(F ${myProject_STYLES})
		INSTALL(FILES "${F}" DESTINATION ./styles)
	ENDFOREACH(F)	

	FILE(GLOB myProject_SETTINGS "${PROJECT_BINARY_DIR}/bin/Release/texttospeech/*.*") 
	FOREACH(F ${myProject_SETTINGS})
		INSTALL(FILES "${F}" DESTINATION ./texttospeech)
	ENDFOREACH(F)	
	
	FILE(GLOB myProject_SETTINGS "${PROJECT_BINARY_DIR}/bin/Release/settings/*.ini") 
	FOREACH(F ${myProject_SETTINGS})
		INSTALL(FILES "${F}" DESTINATION ./settings)
	ENDFOREACH(F)	

#	FILE(GLOB myProject_LANGUAGES "${PROJECT_BINARY_DIR}/bin/Release/languages/*.*") 
#	FOREACH(F ${myProject_LANGUAGES})
#		INSTALL(FILES "${F}" DESTINATION ./languages)
#	ENDFOREACH(F)	


	# For additional libraries just copy and past the following code changing the name of the library
	# remember to check for the right debug suffix - standard are : "d" "_d" 
	#if (library_DIR)
	#  EXTRACT_DEB_REL_DLLS (${library_BINARY_DIR} d )# --> OUR custom macro for searching dlls 
	#  if (ENABLE_verbose)
	#   message (STATUS "\n\n REL DLLS are : ${REL_DLLS}")
	#   message (STATUS "\n\n DEB DLLS are : ${DEB_DLLS}")
	# endif (ENABLE_verbose) 
	#  FOREACH(F ${REL_DLLS})
	#	 INSTALL(FILES "${F}" DESTINATION bin)   # install all release dll
	#  ENDFOREACH(F)
	#  FOREACH(F ${DEB_DLLS})
	#    INSTALL(FILES "${F}" DESTINATION bin)   # no install debug for now
	#  ENDFOREACH(F)
	#endif ()


	#NOTE: this has to go into a separated file script____install.cmake
	#install (TARGETS ${PROJECT_NAME} DESTINATION bin
	#				RUNTIME DESTINATION bin
	#				LIBRARY DESTINATION lib
	#				ARCHIVE DESTINATION lib)
	#install (FILES  "${PROJECT_SOURCE_DIR}/_.h"  DESTINATION include)
	#install (FILES  "${lib_BINARY_DIR}/Release/_.dll"   DESTINATION bin)
					
	if ( ENABLE_verbose )
	  message(STATUS " ---------------------------------- " )
	endif (  )

else (WIN32 AND NOT UNIX)
  message(STATUS " INSTALL NOT YET SUPPORTED " )
endif()