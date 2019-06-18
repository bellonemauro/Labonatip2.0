/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef UPDATER_H_
#define UPDATER_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_updater.h"
#include <QMainWindow>
#include <QTranslator>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>
#include <QtCore>
#include <QtNetwork>
#include <QXmlStreamReader>



QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

using namespace std;

/** \brief Updater class for online updates of biopen wizard
*
*   The procedure implemented here is:
*       1. check on gitHub for update by downloading the information file BpLRMB.txt
*          using the following raw link
*          https://raw.githubusercontent.com/bellonemauro/Labonatip2.0/master/update_data.xml
*       2. extract the following information from the file:
*            a. latest release number
*            b. size of the file to update
*            c. release date
*            d. link to the installer 64bit
*            e. link to the installer 32bit
*       3. the download button is active to make the update available for the user
*       4. download of the installer
*       5. close biopen and lunch the new installation procedure
*
*/
class biopen_updater : public  QMainWindow
{

Q_OBJECT
	QNetworkAccessManager manager;    //!< manager for the network access
	QVector<QNetworkReply *> currentDownloads;  //!< store the current downloads (only one is used so far)

	/** Create signals to be passed to the main app,
	*   the signals allows the class to send information to the main GUI for specific events
	*   the GUI implements a connect to a slot to handle the emitted signals
	*/
	signals :
		void cancel(); //!< signal generated when cancel is pressed
		void exit();   //!< signal generated to close the biopen for the actual update
		void updateAvailable();

public:

	explicit biopen_updater(QWidget *parent = 0); //!< Explicit ctor

	~biopen_updater(); //!< dtor

	/** \brief Switch the language in the GUI
	*
	*  @param _value is the index of the language to load
	*/
	void switchLanguage(QString _translation_file);

	/**  \brief Set the version of the software from the main
	*
	*  @param _version  version to be assigned to the class member m_version
	*/
	void setVersion(QString _version) { m_current_version = _version; }

	/**  \brief Check the internet connection
	*
	*  /return true for success
	*/
	bool isConnectionOk();

	/**  \brief Check for updates
	*
	*  /return true for success
	*/
	bool isUpdateAvailable();

	/**  \brief Download the file at the _url
	*
	*  /return true for success
	*/
	void doDownload(const QUrl &_url);

	/**  \brief Save the filename with full path and return it into a string
	*/
	QString saveFileName(const QUrl &_url);

	/**  \brief Save the downloaded file into the temporary folder
	*/
	bool saveToDisk(const QString &filename, QIODevice *data);

	/**  \brief Detect http redirection
	*/
	static bool isHttpRedirect(QNetworkReply *reply);

	/**  \brief Retrive the redirect target
	*/
	QString getHttpRedirectTarget(QNetworkReply *reply);

	/**  \brief Set verbose to have more output
	*/
	void setVerbose(bool _verbose) { m_verbose = _verbose; }

	/**  \brief Allows to notify official versions only
	*
	*     true = notify all versions
	*     false = notify official releases only
	*/
	void setNotifyExperimental(bool _notify) { m_notify_experimental_ver = _notify; }

public slots:

	/**  \brief Called on download finished it activates the installation procedure
	*/
	void downloadFinished(QNetworkReply *reply);

	/**  \brief Provides information for ssl errors
	*/
	void sslErrors(const QList<QSslError> &errors);

	/**  \brief Start the function to retrieve data from the online folder
	*/
	void startUpdate();

	/**  \brief Check the connection by pinging fluicell.com
	*/
	bool checkConnection();

	/**  \brief Download the installers
	*
	*    \note it automatically detect 32 or 64bit
	*/
	void downloadInstaller();

	/**  \brief Update speed and file size during the download
	*/
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

	/**  \brief Abort active downloads
	*/
	void abortDownload();

	/**  \brief Abort active downloads
	*/
	void cleanTempFolder();


	/**  \brief show details
	*/
	void showDetails();

private:


	/** \brief Ask the password for downloading experimental versions
	*
	*/
	bool askPassword();

	/** \brief Extract useful strings from the downloaded information file
	*
	*/
	bool read_xmlinfo_file(QString _file_path);

	/** \brief After the download take care of the installation procedure, biopen is terminated automatically here
	*
	*/
	bool read_downloaded_installer(QString _file_path);

	/** \brief Initialize all the custom strings 
	*
	*/
	void initCustomStrings();

	/**  \brief Compare current version and online version
	*
	*  Compare current version and online version and return true if 
	*  the online version is higher than the current version
	*
	*  \return true if _current_version < _online_version
	*/
	bool compareVersions(QString _current_version, QString _online_version);

	/**  \brief Update validity check
	*
	*  Used from the main app to check for a valid update
	*  it will call compareVersions(-) and emit the updateAvilable signal 
	*
	*  \return true if a new version is available online
	*/
	bool updateValidityCheck();


    // data member
	QString m_update_info_path;  //!< path to the update information file in the temp folder
	QTimer *m_start_timer;       //!< this timer allows to start automatically the update on_show
	QTimer *m_connection_timer;  //!< check connection and then start the update on_show
	QString m_current_version;   //!< current software version, to be set on class creation from the GUI
	bool is_version_file_ready;  //!< true once the online information is retrieved, false otherwise
	QTime downloadTime;          //!< download time calculated during the download
	QTranslator m_translator_bu; //!< to allow translations
	bool m_details_hiden;        //!< true when details are hidden, false details are shown
	bool m_is_update_available;  //!< set to true if the update is available
	bool m_verbose;              //!< true for more verbose information
	bool m_is_window_active;     //!< true when the window is visualized
	bool m_notify_experimental_ver; //!< if true all versions will be notified

	// information retrieved from the online file
	QString m_online_version;       //!< online software version, retrieved on update check
	QString m_online_version_size;  //!< size of the online version
	QString m_online_version_date;  //!< release data of the online version
	int m_is_experimental;          //|< this is 0 for a normal release, 1 for an experimental release
	QUrl m_url_installer_64bit;     //!< full url of the online installer at 64 bit
	QUrl m_url_installer_32bit;     //!< full url of the online installer at 32 bit

	const QUrl m_fluicell_url;      //!< static url to fluicell website, used for connection verification
	const QUrl m_update_info_url;   //!< static url to the update information file
	const QString m_temp_folder;

	// translatable strings
	QString m_str_warning;
	QString m_str_areyousure;
	QString m_str_information;
	QString m_str_ok;
	QString m_str_no_connection;
	QString m_str_checking;
	QString m_str_checking_connection;
	QString m_str_update_found;
	QString m_str_no_updates;
	QString m_str_downloaded;
	QString m_str_speed;
	QString m_str_file_not_found;
	QString m_str_success_download1;
	QString m_str_success_download2;
	QString m_str_close_biopen;
	QString m_str_check_connection;
	QString m_str_version;
	QString m_str_size;
	QString m_str_released_on;
	QString m_str_not_valid_url;
	QString m_str_download_cancelled;
	QString m_str_xml_problem;
	QString m_str_xml_no_info;

protected:

	/**  \brief Trigger for the show event
	*
	*  The show window event is triggered to allow the update to start
	*  when the user opens the window
	*/
	void showEvent(QShowEvent *ev);

	/** \brief The close event is triggered to pass through the destructor
	*/
	void closeEvent(QCloseEvent *event);

	Ui::Updater *ui_updater;    //!<  the user interface
};


#endif /* UPDATER_H_ */
