![](https://github.com/bellonemauro/Labonatip2.0/blob/master/apps/Lab-on-a-tip/icons/fluicell_logo_BIG.png )

# LAB-ON-A-TIP 2.0

IMPORTANT: This software is still under development !!! There is no release yet, bugs may be everywhere !!!

The Lab-on-a-tip is a software that enables facile configuration and use of the Fluicell <a href="http://fluicell.com/thebiopensystem/">BioPen</a> system. 
The cross-platform BioPen software allows independent control of each pressure line and solution delivery.

Lab-on-a-tip is released under the terms of the <a href="https://www.gnu.org/licenses/gpl-3.0.en.html">GNU GPL</a> license. 


# Dependences

The external dependences for this applications are:
  - <a href="https://github.com/wjwwood/serial">Serial</a> for serial port communication, a modified light version is Included as 3rdParty library. <br>
    NOTE: The official version available on github will not work.
  - QT  v. >5.4, for GUI - https://www.qt.io/
  - nsis http://nsis.sourceforge.net/Main_Page - only to generate the installation package
  - <a href="http://www.cmake.org">CMake</a> to generate the solution for your system and build. 




# Fetching instructions:

From git shell write:

    git clone https://github.com/bellonemauro/Labonatip2.0.git  your_folder

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
in the case you don't have environmental variables set you should find manually some dependences path:
" \__\" folder, in case of QT libraries, C:/___/Qt/__your_version___/__compiler__ersion__/lib/cmake/Qt5_library 

in my experience everything is automatically found 

configure --->  generate and open visual studio

to build the package in windows : windeployqt yourfile.exe

![](https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipWin.png)


<b> Linux </b>

CMake --> configure --> generate 

Add a screen shot
![](https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipUbuntu1.png )
![](https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipUbuntu3.png )
![](https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipUbuntu4.png )

TODO

---------------------------------------------------------------------
<sup> Software released under GNU GPL License. <br>
Author: <a href="http://fluicell.com/">Fluicell</a> AB , Mauro Bellone, http://www.maurobellone.com <br> </sup>