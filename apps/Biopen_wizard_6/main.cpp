/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

//uncomment to hide the console when the app starts
#ifndef _DEBUG
#define HIDE_TERMINAL 
#endif
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
#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QScreen>


// if it is the first time that the software runs,
// it will check if required paths already exist and 
// set up useful files and folders in the user files
// if this function return false, some path may be broken
bool initPaths(Labonatip_GUI &_l, QString &_protocols_user_path, 
	QString &_settings_user_path, QString &_ext_data_user_path)
{
	// detect the home path ... C:/users/user/
	QString home_path = QDir::homePath();   

	// is the installation folder  ... C:/Program Files/Biopen2
	QDir app_dir = QDir::currentPath();    
	
	// default protocol path into the installation folder
	QString protocols_path = app_dir.path();    
	protocols_path.append("/presetProtocols/");

	// default setting path into the installation folder
	QString settings_path = app_dir.path(); 
	settings_path.append("/settings/");

	// default ext_data path into the installation folder
	QString ext_data_path = app_dir.path();
	ext_data_path.append("/Ext_data/");

	// if the directory Biopen does not exist in the home folder, create it
	home_path.append("/Documents/Biopen6/");
	QDir home_dir;
	if (!home_dir.exists(home_path)) {
		std::cerr << " BiopenWizard directory does not exists in the home folder .... creating it" << std::endl;
		home_dir.mkpath(home_path);
		std::cout << " Created directory " <<
            home_path.toStdString() << std::endl;
	}
	else {
		std::cout << " Found directory " <<
            home_path.toStdString() << std::endl;
	}

	// check if the protocol directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir protocols_dir;
	protocols_dir.setPath(protocols_path);
	if (!protocols_dir.exists(protocols_path) ) {
		std::cerr << "ERROR: Biopen protocols directory does not exists in the installation folder"
			 << "A reinstallation may solve the problem "<< std::endl;
		QString ss = "Protocols directory does not exists in the installation folder,";
		ss.append("Biopen wizard cannot run  <br>"); 
		ss.append ("A reinstallation of Biopen wizard may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		std::cout << " Found directory " <<
            protocols_path.toStdString() << std::endl;
	}

	// check if the settings directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir settings_dir;
	settings_dir.setPath(settings_path);
	if (!settings_dir.exists(settings_path)) {
		std::cerr << "Biopen wizard settings directory does not exists" << std::endl;
		QString ss = "Settings directory does not exists in the installation folder,";
		ss.append("Biopen wizard cannot run  <br>");
		ss.append("A reinstallation of Biopen wizard may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		std::cout << " Found directory " <<
            settings_path.toStdString() << std::endl;
	}

	// check if the ext_data directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir ext_data_dir;
	ext_data_dir.setPath(ext_data_path);
	if (!ext_data_dir.exists(ext_data_path)) {
		std::cerr << "Biopen wizard ext_data directory does not exists" << std::endl;
		QString ss = "Ext_data directory does not exists in the installation folder,";
		ss.append("Biopen wizard cannot run  <br>");
		ss.append("A reinstallation of Biopen wizard may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		std::cout << " Found directory " <<
            ext_data_path.toStdString() << std::endl;
	}

	// here we set the macro path in the user folder 
	QDir protocols_user_dir;
	QString protocols_home_path = home_path;
	protocols_home_path.append("/presetProtocols/");
	if (!protocols_user_dir.exists(protocols_home_path)) // if the macro user folder does not exist, create and copy
	{
		_protocols_user_path = protocols_home_path;
		if (!protocols_user_dir.mkpath(_protocols_user_path))
		{
			std::cerr << "Could not create presetProtocols folder in the user directory" << std::endl;
			QString ss = "Could not create presetProtocols folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

	}
	else {
		_protocols_user_path = protocols_home_path;
	}

	// directory exists, copy files 
	{
		QStringList filesList = protocols_dir.entryList(QDir::Files);
		std::cout << "filesList info, protocols folder contains "
			<< filesList.size() << " files " << std::endl;

		QString file_name;
		foreach(file_name, filesList)
		{
			QFile file1(_protocols_user_path + file_name);
			QFile file2(protocols_path + file_name);
			if (!file1.exists())
				QFile::copy(file2.fileName(), file1.fileName());
		}
	}

	// here we set the setting path in the user folder 
	QDir settings_user_dir;
	QString settings_home_path = home_path;
	settings_home_path.append("/settings/");
	if (!settings_user_dir.exists(settings_home_path))
	{
		_settings_user_path = settings_home_path;
		if (!settings_user_dir.mkpath(_settings_user_path)) {
			std::cerr << "Could not create settings folder in the user directory" << std::endl;
			QString ss = "Could not create settings folder in the user directory";
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
		std::cout << "filesList info, setting folder contains "
			 << filesList.size() << " files " << std::endl;

		QString file_name;
		foreach(file_name, filesList)
		{
			QFile file1(_settings_user_path + file_name);
			QFile file2(settings_path + file_name);
			if (!file1.exists())
				QFile::copy(file2.fileName(), file1.fileName());
		}
	}


	// here we set the ext data path in the user folder 
	QDir ext_data_user_dir;
	QString ext_data_home_path = home_path;
	ext_data_home_path.append("/Ext_data/");
	if (!ext_data_user_dir.exists(ext_data_home_path))
	{
		_ext_data_user_path = ext_data_home_path;
		if (!ext_data_user_dir.mkpath(_ext_data_user_path)) {
			std::cerr << "Could not create ext_data folder in the user directory" << std::endl;
			QString ss = "Could not create ext_data folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

	}
	else {
		_ext_data_user_path = ext_data_home_path;
	}

	return true;
}


int main(int argc, char **argv)
{	

	// get the version 
	std::string version;
#ifdef LABONATIP_VERSION
	version = VER;
	std::cout << "\n Running Lab-on-a-tip version "
         << version << "\n"<< endl;
#endif
	try {

        QApplication a (argc, argv);
        // there is a problem with high dpi displays
        a.setAttribute(Qt::AA_EnableHighDpiScaling);
		
		Labonatip_GUI window;

		// check for high DPI screens
		int logical_dpi_x = QApplication::desktop()->logicalDpiX();
		int logical_dpi_y = QApplication::desktop()->logicalDpiY();
		int physical_dpi_x = QApplication::desktop()->physicalDpiX();
		int physical_dpi_y = QApplication::desktop()->physicalDpiY();

		// get the screen resolution of the current screen
		// so we can resize the application in case of small screens
	    QScreen *primaryScreen = QGuiApplication::primaryScreen();
		QRect rec = primaryScreen->geometry();
		int screen_height = rec.height();
		int screen_width = rec.width();

		std::cout << " Labonatip_GUI::main ::: "
			<< " logical_dpi_x " << logical_dpi_x
			<< " logical_dpi_y " << logical_dpi_y
			<< " physical_dpi_x " << physical_dpi_x
			<< " physical_dpi_y " << physical_dpi_y 
			<< " screen_height " << screen_height 
			<< " screen_width " << screen_width << std::endl;

		if (logical_dpi_x > 150) {
			QString ss = "Your display DPI is out of bound for the correct visualization of Biopen wizard\n";
			ss.append("You can continue, but you will probably get bad visualization \n\n");
			ss.append("To properly visualize Biopen wizard, try to reduce the resolution and scaling of your screen");
			
			QMessageBox::warning(&window, "ERROR", ss);

			window.appScaling(logical_dpi_x, logical_dpi_y);
			window.setGeometry(50, 50, screen_width*0.8, screen_height*0.8);
			
		}
        

        // set internal application paths
        QString protocols_user_path;
        QString settings_user_path;
        QString ext_data_user_path;

#ifdef _DEBUG
        std::cout << " Running with debug settings " << std::endl;
        initPaths(window, protocols_user_path,
            settings_user_path, ext_data_user_path);
#else
        if (!initPaths(window, protocols_user_path,
            settings_user_path, ext_data_user_path)) return 0;
#endif

      // set default paths for settings and protocols in the GUI app
	  window.setProtocolUserPath(protocols_user_path);
	  std::cout << " Set protocols_user_path "
		  << protocols_user_path.toStdString() << std::endl;
	  window.setSettingsUserPath(settings_user_path);
	  std::cout << " Set settings_user_path "
		  << settings_user_path.toStdString() << std::endl;
	  window.setExtDataUserPath(ext_data_user_path);
	  //window.setExtDataUserPath("./Ext_data/");  // this is just for now to be taken out for the release
	  std::cout << " Set ext_data_user_path "
		  << ext_data_user_path.toStdString() << std::endl;

#ifdef LABONATIP_VERSION
	  window.setVersion(version);
#endif

	  // show the slash screen
	  QSplashScreen s;
	  s.setPixmap(QPixmap(":/icons/splash_screen.png"));
	  s.show();
	  QTimer::singleShot(5000, &s, SLOT(close()));

	  

	  //window.showFullScreen();
	  window.move(QPoint(50, 50));
	  if (screen_width < 1400)
		 QTimer::singleShot(5000, &window, SLOT(showMaximized()));
	  else
		  QTimer::singleShot(5000, &window, SLOT(show()));
  
	  return a.exec ();
  }
  catch (std::exception &e) {
	  std::cerr << " Labonatip_GUI::main ::: Unhandled Exception: "
		   << e.what() << endl;
	  // clean up here, e.g. save the session, save the current protocol
	  // and close all config files.
	  std::cout << " Something really bad just happened, press ok to exit "
		   << endl;
#ifndef HIDE_TERMINAL
	  std::cin.get();
#endif
	  return 0; // exit the application
  }

  return 0; // exit the application
}
