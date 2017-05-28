
Fluicell AB - Lab-on-a-tip                                                              
Copyright 2017 © Fluicell AB, http://fluicell.com/                                          

Developer: Mauro Bellone, http://www.maurobellone.com                       
Released under GNU GPL License.                                               


This application has been developed for research purposes.

The external dependences for this applications are:
  QT  v. >5.4
  serial --- Included as 3rdParty package

The following libraries are included into the distribution but they can optionally
compliled from external source:


nsis - only to generate the installation package

API documentation available at https://github.com/bellonemauro/PPC1API-docs.io/settings

known issues:
  - none for now

	
============================================================================
# Fetching instructions:

From git shell write:

    git clone https://github.com/bellonemauro/Labonatip.git your_folder

then you should get something like:

	C:\...\Labonatip> git clone https://github.com/bellonemauro/Labonatip.git ./
	Cloning into '.'...
	remote: Counting objects: 7, done.
	remote: Compressing objects: 100% (7/7), done.
	remote: Total 7 (delta 1), reused 0 (delta 0), pack-reused 0
	Unpacking objects: 100% (7/7), done.
	Checking connectivity... done.

done !!! 

============================================================================
# Building Instructions:

IMPORTANT: set your build folder OUT of the code folder,  
           the build must be independent from the code 

Use CMake to generate the solution for your system, see http://www.cmake.org

in the case you don't have environmental variables set you should find manually some dependences:
" \__\" folder 

in my experience everything is automatically found 

configure --->  generate and open visual studio

to build the package in windows : windeployqt yourfile.exe
