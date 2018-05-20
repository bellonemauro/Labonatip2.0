Here we have all the translations for Labonatip2. 

The user readable translation are in files renominated as ___.ts

Use QTlinguist http://doc.qt.io/qt-5/qtlinguist-index.html 
to create the .qm file comprehensible for the application

Follow this procedure: 
Open QTlinguist 
-> open .\Labonatip\apps\Lab-on-a-tip\languages\file.ts  
-> releaseAs   sameFolder/sameName.qm

command example: 
%QT5_BINARY_DIR%\lrelease.exe .\eng.ts .\eng.qm

One can also use the tranlateAll.bat file to generate everything in the same folder
ATTENTION: tranlateAll.bat contains a static link to linguist !!!!
            If it does not work the variable QT5_BINARY_DIR need to be set to the proper location


this will be changed in the future to automatically add the translations:
see https://stackoverflow.com/questions/44782914/cmake-qt5-add-translation-how-to-specify-the-output-path