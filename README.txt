
  |                              Fluicell AB                                  |
  |                                                                           |
  | Lab-on-a-tip                                                              |
  |                                                                           |
  | Copyright 2017 © Fluicell AB                                              |
  | Authors: Mauro Bellone, http://www.maurobellone.com                       |
  | Released under ___ License.                                               |
  |                                                                           |


This application has been developed for research purposes.

The external dependences for this applications are:
  QT  v. >5.4
  serial --- Included as 3rdParty package

The following libraries are included into the distribution but they can optionally
compliled from external source:


nsis - only to generate the installation package


known issues:
  - none for now

	
============================================================================
# Fetching instructions:

From git shell write:

    git clone https://___ your_folder

then you should get something like:

    Cloning into 'your_folder'...
    Username for 'https://bitbucket.org': your_username 
    Password for 'https://bellonemauro@gmail.com@bitbucket.org':    your_password  
    remote: Counting objects: 45, done.
    remote: Compressing objects: 100% (41/41), done.
    remote: Total 45 (delta 13), reused 0 (delta 0)
    Unpacking objects: 100% (45/45), done.
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
