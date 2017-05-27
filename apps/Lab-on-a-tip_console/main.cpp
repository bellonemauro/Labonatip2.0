/*  +---------------------------------------------------------------------------+
 *  |                                                                           |
 *  |                      http://www.maurobellone.com                          |
 *  |                                                                           |
 *  | Copyright (c) 2017, - All rights reserved.                                |
 *  | Authors: Mauro Bellone                                                    |
 *  | Released under ___ License.                                               |
 *  +---------------------------------------------------------------------------+ */

//uncomment to hide the consolle when the app starts
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "Lab-on-a-tip_console.h"

  
int main (int argc, char *argv[])
{	
  QApplication a (argc, argv);
  Labonatip_console window;

  //window.showFullScreen();
  //window.showMaximized();
  window.show ();
  return a.exec ();
}
