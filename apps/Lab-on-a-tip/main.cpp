/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

//uncomment to hide the console when the app starts
//#define HIDE_TERMINAL
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
#include <QSplashScreen>
#include <QTimer>
#include <QDir>


// if it is the first time that the software runs,
// it will set up useful files and folders in the user files
void setPaths()
{
	QString home_path = QDir::homePath();
	QDir app_dir = QDir::currentPath();
	QString macro_path = app_dir.path();
	macro_path.append("/presetMacros/");

	QDir macro_dir;
	if (!macro_dir.exists(macro_path))
	{
		cerr << "Labonatip macro directory does not exists" << endl;
	}
	

	home_path.append("/Labonatip/");
	QDir home_dir;
	QDir macro_user_dir;
	if (!home_dir.exists(home_path))
	{
		home_dir.mkpath(home_path);
		cout << "directory " << home_path.toStdString() << " now exists" << endl;


		QString macro_user_path = home_path;
		macro_user_path.append("/presetMacros/");
		macro_user_dir.mkpath(macro_user_path);
		cout << "directory " << macro_user_path.toStdString() << " now exists" << endl;
		
		QFile file1(macro_dir.path() + "/initialize.macro");
		QFile file2(macro_path + "/initialize.macro");
		QFile::copy(file1.fileName(), file2.fileName());

		//if (file1.open(QIODevice::ReadWrite))
		//{
		//	cout << "file now exists";
		//}
	}

}


int main(int argc, char **argv)//(int argc, char *argv[])
{	

	//setPaths();

	string version;
#ifdef LABONATIP_VERSION
	version = VER;
	cout << " Running Lab-on-a-tip version " << version << endl;
#endif
	try {

  QApplication a (argc, argv);
  Labonatip_GUI window;

  QSplashScreen *s = new QSplashScreen();
  s->setPixmap(QPixmap("./icons/splash_screen.png"));
  s->show();
  QTimer::singleShot(5000, s, SLOT(close()));

#ifdef LABONATIP_VERSION
  window.setVersion(version);
#endif

  // get the screen resolution of the current screen
  // so we can resize the application in case of small screens
  QRect rec = QApplication::desktop()->screenGeometry();
  int screen_height = rec.height();
  int screen_width = rec.width();

  //window.showFullScreen();
  //window.showMaximized();
  window.move(QPoint(50, 50));
  //window.show ();
  if (screen_width < 1400)
	 QTimer::singleShot(5000, &window, SLOT(showMaximized()));
  else
	  QTimer::singleShot(5000, &window, SLOT(show()));
  
  //return a.exec ();
  
  
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



