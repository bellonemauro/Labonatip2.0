# LAB-ON-A-TIP 2.0



The Lab-on-a-tip is a ....

Lab-on-a-tip is released under the terms of the ____ license, and thus free/not-free for commercial and research use. 


# Dependences

The external dependences for this applications are:
  - <a href="https://github.com/wjwwood/serial">Serial</a> for serial port communication, a modified light version is Included as 3rdParty library. <br>
    NOTE: The official version available on github will not work.
  - QT  v. >5.4, for GUI - https://www.qt.io/
  - nsis http://nsis.sourceforge.net/Main_Page - only to generate the installation package
  - <a href="http://www.cmake.org">CMake</a> to generate the solution for your system and build. 

The following libraries are included into the distribution, but they can optionally compliled from external source:
  - <a href="http://eigen.tuxfamily.org/index.php?title=Main_Page">Eigen library</a>, for matrix computation




# Fetching instructions:

From git shell write:

    git clone https://___ your_folder

then you should get something like:

    Cloning into 'your_folder'...
    Username for 'https://github.com/': your_username 
    Password for '  your_username  ':    your_password  
    remote: Counting objects: 45, done.
    remote: Compressing objects: 100% (41/41), done.
    remote: Total 45 (delta 13), reused 0 (delta 0)
    Unpacking objects: 100% (45/45), done.
    Checking connectivity... done.

done !!! 


# Building Instructions:

Use CMake to generate the solution for your system, see http://www.cmake.org

It is advised to set your build folder OUT of the source code folder, the build should be independent from the code. 

<b> Windows </b><br>
in the case you don't have environmental variables set you should find manually some dependences:
" \__\" folder 

in my experience everything is automatically found 

configure --->  generate and open visual studio

to build the package in windows : windeployqt yourfile.exe

<b> Linux </b>

CMake --> configure --> generate 

TODO

---------------------------------------------------------------------
<sup>Copyright 2017 © <a href="http://fluicell.com/">Fluicell</a> AB <br>
Author: Mauro Bellone, http://www.maurobellone.com <br>
Released under GNU LGP3 LICENSE License. </sup>