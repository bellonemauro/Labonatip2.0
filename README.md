<a href="http://fluicell.com/"><img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/apps/Lab-on-a-tip/icons/fluicell_logo_BIG.png"  width="200" height="70" /></a>

# LAB-ON-A-TIP 2.0

IMPORTANT: This software is still under development !!! There is no release yet, bugs may be everywhere !!!

The Lab-on-a-tip is a software that enables facile configuration and use of the <a href="http://fluicell.com/">Fluicell</a> <a href="http://fluicell.com/thebiopensystem/">BioPen</a> system. 
The cross-platform BioPen software allows independent control of each pressure line and solution delivery.

Lab-on-a-tip is released under the terms of the <a href="https://www.gnu.org/licenses/gpl-3.0.en.html">GNU GPL</a> license. 

# Dependences

The external dependences for this applications are:
  - <a href="https://github.com/wjwwood/serial">Serial</a> for serial port communication, a modified light version is Included as 3rdParty library. <br>
    NOTE: The official version of serial library available on github will not work, refer to the third party folder in this package.
  - QT  v. >5.4, for GUI - https://www.qt.io/
  - nsis http://nsis.sourceforge.net/Main_Page - only to generate the installation package
  - <a href="http://www.cmake.org">CMake</a> to generate the solution for your system and build. 
  - To install on some previous windows version you may need to update your Universal C Runtime in Windows <a href="https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows">here</a>


# Building Instructions:

Use CMake to generate the solution for your system, see http://www.cmake.org
It is advised to set your build folder OUT of the source code folder, the build should be independent from the code. 


## Windows 
in the case you don't have environmental variables set you should find manually some dependences path:
" \__\" folder, in case of QT libraries, C:/___/Qt/__your_version___/__compiler__ersion__/lib/cmake/Qt5_library 

In my experience everything is automatically found, but sometimes some QT specific package folder needs to be set manually, I will solve this soon! 

configure --->  generate

Io build the package in windows the varialble WINDDEPLOYQT_EXECUTABLE need to point to "your_qt_folder/bin/windeployqt.exe "


<img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipWin.png"  width="380" height="230" /> <img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipWin1.png"  width="380" height="230" /> 
<img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipWin2.png"  width="380" height="230" /> <img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipWin3.png"  width="380" height="230" />

## Linux 

CMake --> configure --> generate 

Add a screen shot

<img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipUbuntu1.png"  width="380" height="230" />  <img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipUbuntu3.png"  width="380" height="230" />
<img src="https://github.com/bellonemauro/Labonatip2.0/blob/master/Ext_data/labonatipUbuntu4.png"  width="380" height="230" />


# Contribute to the code:

New developers can contribute to the code by compiling in Windows or linux.
QT GUI must be modified using QT creator/designer


## Known issues
The uninstaller does not completely remove the start-menu folder sometimes, 
if it happens, remove manually from C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Lab-on-a-tip______

---------------------------------------------------------------------
<sup> Software released under GNU GPL License. <br>
Author: <a href="http://fluicell.com/">Fluicell</a> AB , Mauro Bellone, http://www.maurobellone.com <br> </sup>
