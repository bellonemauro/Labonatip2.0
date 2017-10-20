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
#include <QMessageBox>


// if it is the first time that the software runs,
// it will check if required paths already exist and set up useful files and folders in the user files
// if this function return false, some path may be broken
bool setPaths(Labonatip_GUI &_l, QString &_macro_user_path, QString &_settings_user_path )
{
	QString home_path = QDir::homePath();   // detect the home path ... C:/users/user/
	QDir app_dir = QDir::currentPath();     // is the installation folder  ... C:/Program Files/Labonatip
	QString macro_path = app_dir.path();    // default macros path into the installation folder
	macro_path.append("/presetMacros/");   
	QString settings_path = app_dir.path(); // default setting path into the installation folder
	settings_path.append("/settings/");

	// if the directory Labonatip does not exist in the home folder, create it
	home_path.append("/Labonatip/");
	QDir home_dir;
	if (!home_dir.exists(home_path)) {
		cerr << "Labonatip directory does not exists in the home folder .... creating it" << endl;
		home_dir.mkpath(home_path);
		cout << "directory " << 
			home_path.toStdString() << " now exists" << endl;
	}
	else {
		cout << "directory " << 
			home_path.toStdString() << " already exists" << endl;
	}

	// check if the macro directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir macro_dir;
	macro_dir.setPath(macro_path);
	if (!macro_dir.exists(macro_path) ) {
		cerr << "ERROR: Labonatip macro directory does not exists in the installation folder"
			 << " reinstallation may solve the problem "<< endl;
		QString ss = "Macro directory does not exists in the installation folder,";
		ss.append("Labonatip cannot run  <br>"); 
		ss.append ("A reinstallation of Labonatip may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		cout << "directory " << 
			macro_path.toStdString() << " exists" << endl;
	}

	// check if the settings directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir settings_dir;
	settings_dir.setPath(settings_path);
	if (!settings_dir.exists(settings_path)) {
		cerr << "Labonatip settings directory does not exists" << endl;
		QString ss = "Settings directory does not exists in the installation folder,";
		ss.append("Labonatip cannot run  <br>");
		ss.append("A reinstallation of Labonatip may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		cout << "directory " << 
			settings_path.toStdString() << " exists" << endl;
	}


	// here we set the macro path in the user folder 
	QDir macro_user_dir;
	QString macro_home_path = home_path;
	macro_home_path.append("/presetMacros/");
	if (!macro_user_dir.exists(macro_home_path)) // if the macro user folder does not exist, create and copy
	{
		_macro_user_path = macro_home_path;
		if (!macro_user_dir.mkpath(_macro_user_path))
		{
			cerr << "Could not create presetMacros folder in the user directory" << endl;
			QString ss = "Could not create presetMacros folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

	}
	else {
		_macro_user_path = macro_home_path;
	}

	// directory exists, copy files 
	{
		QStringList filesList = macro_dir.entryList(QDir::Files);
		cout << "filesList info, macro folder contains " << filesList.size() << " files " << endl;

		QString file_name;
		foreach(file_name, filesList)
		{
			QFile file1(_macro_user_path + file_name);
			QFile file2(macro_path + file_name);
			if (!file1.exists())
				QFile::copy(file2.fileName(), file1.fileName());
		}
	}

	QDir settings_user_dir;
	QString settings_home_path = home_path;
	settings_home_path.append("/settings/");
	if (!settings_user_dir.exists(settings_home_path))
	{
		_settings_user_path = settings_home_path;
		if (!settings_user_dir.mkpath(_settings_user_path)) {
			cerr << "Could not create settings folder in the user directory" << endl;
			QString ss = "Could not create presetMacros folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}
		
	}
	else {
		_settings_user_path = settings_home_path;

	}

	// directory exists, copy files
	{
		_settings_user_path = settings_home_path;

		QStringList filesList = settings_dir.entryList(QDir::Files);
		cout << "filesList info, setting folder contains " << filesList.size() << " files " << endl;

		QString file_name;
		foreach(file_name, filesList)
		{
			QFile file1(_settings_user_path + file_name);
			QFile file2(settings_path + file_name);
			if (!file1.exists())
				QFile::copy(file2.fileName(), file1.fileName());
		}
	}

	return true;
}


int main(int argc, char **argv)//(int argc, char *argv[])
{	
	// get the version 
	string version;
#ifdef LABONATIP_VERSION
	version = VER;
	cout << " Running Lab-on-a-tip version " << version << endl;
#endif
	try {

	  QApplication a (argc, argv);
	  Labonatip_GUI window;

	  QString macro_user_path;
	  QString settings_user_path;

	  if (!setPaths(window, macro_user_path, settings_user_path)) return 0;

#ifdef LABONATIP_VERSION
	  window.setVersion(version);
#endif

	  // show the slashscreen
	  QSplashScreen *s = new QSplashScreen();
	  s->setPixmap(QPixmap("./icons/splash_screen.png"));
	  s->show();
	  QTimer::singleShot(5000, s, SLOT(close()));

	  // set default paths for settings and macros
	  window.setMacroUserPath(macro_user_path);
	  window.setSettingsUserPath(settings_user_path);

	  // get the screen resolution of the current screen
	  // so we can resize the application in case of small screens
	  QRect rec = QApplication::desktop()->screenGeometry();
	  int screen_height = rec.height();
	  int screen_width = rec.width();

	  //window.showFullScreen();
	  window.move(QPoint(50, 50));
	  if (screen_width < 1400)
		 QTimer::singleShot(5000, &window, SLOT(showMaximized()));
	  else
		  QTimer::singleShot(5000, &window, SLOT(show()));
  
	  return a.exec ();
  }
  catch (std::exception &e) {
	  cerr   << " Labonatip_GUI::main ::: Unhandled Exception: " << e.what() << endl;
	  // TODO: clean up here, e.g. save the session
	  // and close all config files.
	  std::cout << " Something really bad just happend, press ok to exit " << std::endl;
	  std::cin.get();
	  return 0; // exit the application
  }

  return 0; // exit the application
}
