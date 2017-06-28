/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

//uncomment to hide the console when the app starts
#define HIDE_TERMINAL
#ifdef HIDE_TERMINAL
	#if defined (_WIN64) || defined (_WIN32)
	  #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#else
	// define it for a Unix machine
	#endif
#endif

// extract the version string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define VER STR(LABONATIP_VERSION)

#include "Lab-on-a-tip.h"



int main(int argc, char **argv)//(int argc, char *argv[])
{	
	string version;
#ifdef LABONATIP_VERSION
	version = VER;
	cout << " Running Lab-on-a-tip version " << version << endl;
#endif
		
  QApplication a (argc, argv);
  Labonatip_GUI window;

#ifdef LABONATIP_VERSION
  window.setVersion(version);
#endif

  //window.showFullScreen();
  //window.showMaximized();
  window.move(QPoint(50, 50));
  window.show ();
  //return a.exec ();
  
  try { 
	  a.exec();
  }
  catch (std::exception &e) {
	  cerr   << " Labonatip_GUI::main ::: Unhandled Exception: " << e.what() << endl;
	  // clean up here, e.g. save the session
	  // and close all config files.
	  std::cout << " Something really bad just happend, press ok to exit " << std::endl;
	  std::cin.get();
	  return 0; // exit the application
  }

  return 0; // exit the application
}



