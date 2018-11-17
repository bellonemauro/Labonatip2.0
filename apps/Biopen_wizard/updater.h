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
#include <QtNetwork\QtNetwork>




QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

using namespace std;

/** \brief Labonatip_tools class for settings
*
*   Tools open a new window to allow the user to choose settings. 
*   There are 4 main subsection:
*      - General settings : 
*      - Solution settings :
*      - Pressure and vacuum settings :
*      - Communication settings :
*
*   For each of this section a data structure is defined in the 
*   header dataStructure.h
*
*/
class biopen_updater : public  QMainWindow
{
	Q_OBJECT
	QNetworkAccessManager manager;
	QVector<QNetworkReply *> currentDownloads;

	/** Create signals to be passed to the main app,
	*   the signals allows the tools class to send information to the main class for specific events
	*   the main class implements a connect to a slot to handle the emitted signals
	* 
	*/
	signals :
		void cancel(); //!< signal generated when cancel is pressed
		void exit();

public:

	explicit biopen_updater(QWidget *parent = 0); //!< Explicit ctor

	~biopen_updater(); //!< dtor

	void switchLanguage(QString _translation_file);

	/**  \brief Set the version of the software from the main
	*
	*  @param _version  version to be assigned to the class member m_version
	*/
	void setVersion(QString _version) { m_current_version = _version; }

	bool isConnectionOk();

	void doDownload(const QUrl &url);

	static QString saveFileName(const QUrl &url);

	bool saveToDisk(const QString &filename, QIODevice *data);

	static bool isHttpRedirect(QNetworkReply *reply);

	QString getHttpRedirectTarget(QNetworkReply *reply);


public slots:
	void downloadFinished(QNetworkReply *reply);
	void sslErrors(const QList<QSslError> &errors);
	void startUpdate();
	void downloadInstaller();
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:

	bool read_info_file(QString _file_path);
	bool read_downloaded_installer(QString _file_path);
	void initCustomStrings();

	bool compareVersions(QString _current_version, QString _online_version);

	QString m_update_info_path;
	QTimer *m_start_timer;
	QString m_tmp_folder_name;
	QString m_current_version;
	QString m_online_version;
	QString m_online_version_size;
	QString m_online_version_date;
	QUrl m_url_installer_64bit;
	QUrl m_url_installer_32bit;
	bool is_version_file_ready;
	QTime downloadTime;
	QTranslator m_translator_bu;

	// translatable strings
	QString m_str_warning;
	QString m_str_areyousure;
	QString m_str_information;
	QString m_str_ok;
	QString m_str_checking;
	QString m_str_update_found;
	QString m_str_no_updates;
	QString m_str_downloaded;
	QString m_str_speed;
	QString m_str_file_not_found;
	QString m_str_success_download1;
	QString m_str_success_download2;
	QString m_str_close_biopen;

protected:
	void showEvent(QShowEvent *ev);

	Ui::Updater *ui_updater;    //!<  the user interface
};


#endif /* UPDATER_H_ */
