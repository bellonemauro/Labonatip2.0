/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "updater.h"
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QInputDialog>

biopen_updater::biopen_updater(QWidget *parent):
	QMainWindow(parent),
	ui_updater(new Ui::Updater),
	m_verbose(false),
	m_notify_experimental_ver(false),
	m_temp_folder("/biopen_tmp"),
	m_is_window_active(false),
	m_fluicell_url("http://fluicell.com/"),
	m_update_info_url("https://raw.githubusercontent.com/bellonemauro/Labonatip2.0/master/update_data.xml")
{
	//Main things to add: 
	// 1. trigger the exit event to avoid pending download

		ui_updater->setupUi(this );
		this->initCustomStrings();

		m_update_info_path = "";
		m_current_version = "";
		m_online_version = "";
		m_online_version_size = "";
		m_online_version_date = "";
		m_details_hiden = false;

		m_start_timer = new QTimer();
		m_start_timer->setInterval(2000);
		m_connection_timer = new QTimer();
		m_connection_timer->setInterval(100);

		m_url_installer_64bit = "";
		m_url_installer_32bit = "";
		is_version_file_ready = false;

		connect(m_connection_timer,
			SIGNAL(timeout()), this,
			SLOT(checkConnection()));

		connect(m_start_timer,
			SIGNAL(timeout()), this,
			SLOT(startUpdate()));
	
		connect(ui_updater->pushButton_download,
			SIGNAL(clicked()), this,
			SLOT(downloadInstaller()));	
		
		connect(ui_updater->pushButton_details,
			SIGNAL(clicked()), this,
			SLOT(showDetails()));

		connect(ui_updater->pushButton_cancelDownload,
			SIGNAL(clicked()), this,
			SLOT(abortDownload()));

		connect(&manager, SIGNAL(finished(QNetworkReply*)),
			SLOT(downloadFinished(QNetworkReply*)));
}


void biopen_updater::showEvent(QShowEvent *ev)
{
	QMainWindow::showEvent(ev);

	//make sure that the GUI is clear and ready for the new check
	ui_updater->textEdit_details->clear();
	ui_updater->label_title->setText(m_str_checking_connection); 
	ui_updater->pushButton_download->setEnabled(false);
	ui_updater->label_icon->setEnabled(false);

	// turn off the download and cancel button
	ui_updater->pushButton_download->setEnabled(false);
	ui_updater->pushButton_cancelDownload->setEnabled(false);
	is_version_file_ready = false;
	
	// clean the labels
	ui_updater->label_download_status->setText(" ");
	ui_updater->label_version->setText("");
	ui_updater->label_size->setText("");
	ui_updater->label_releaseDate->setText("");

	// hide details
	if (!m_details_hiden)//(ui_updater->pushButton_details->isChecked());
	{
		ui_updater->pushButton_details->blockSignals(true);
		ui_updater->pushButton_details->setChecked(false);
		ui_updater->pushButton_details->blockSignals(false);
		int gb_size_w = ui_updater->textEdit_details->width();
		int gb_size_h = ui_updater->textEdit_details->height();
		ui_updater->groupBox->hide();
		m_details_hiden = true;

		this->resize(this->width(), this->height() - gb_size_h);
	}

	m_is_window_active = true;
	// this timer is required to start the actual online request
	m_connection_timer->start();
}


bool biopen_updater::isConnectionOk()
{
	m_connection_timer->stop();
	QNetworkAccessManager nam;
	QNetworkRequest req(m_fluicell_url);
	QNetworkReply *reply = nam.get(req);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	if (reply->bytesAvailable()) {
		ui_updater->label_icon->setEnabled(true); 
		return true;
	}
	else {
		ui_updater->label_icon->setEnabled(false);
		return false;
	}
}

bool biopen_updater::isUpdateAvailable()
{
	if (this->isConnectionOk())
	{
		this->doDownload(m_update_info_url);
	}
	return false;
}


void biopen_updater::doDownload(const QUrl & _url)
{

	QNetworkRequest request(_url);
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	QNetworkReply *reply = manager.get(request);

	if (m_verbose) ui_updater->textEdit_details->append("Downloading file from: ");
	if (m_verbose) ui_updater->textEdit_details->append(_url.toString());

#if QT_CONFIG(ssl)
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
		SLOT(sslErrors(QList<QSslError>)));
#endif
	
	currentDownloads.append(reply);
	connect(currentDownloads.at(0), SIGNAL(downloadProgress(qint64, qint64)),
		SLOT(downloadProgress(qint64, qint64)));
	downloadTime.start();
	ui_updater->pushButton_cancelDownload->setEnabled(true);
}


void biopen_updater::abortDownload()
{
	if (currentDownloads.size() > 0)
	{
		currentDownloads.at(0)->abort(); // for now we are sure that we do not have more than one
		ui_updater->pushButton_cancelDownload->setEnabled(false);
	}
}

void biopen_updater::cleanTempFolder()
{
	QString save_path = QDir::tempPath();
	save_path.append(m_temp_folder); // temporary folder for download
	// if the temp folder exist we can clean it for the next download
	QDir dir(save_path);
	// list all the file names
	dir.setNameFilters(QStringList() << "*.*");
	dir.setFilter(QDir::Files);
	// remove the files one by one
	foreach(QString dirFile, dir.entryList())
	{
		dir.remove(dirFile);
	}
}

void biopen_updater::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{

	//progressBar.setStatus(bytesReceived, bytesTotal);
	QString status_msg;
	status_msg.append(m_str_downloaded);
	status_msg.append(" ");
	double br = bytesReceived / 100000;
	br = br / 10.0;
	status_msg.append(QString::number(br));
	status_msg.append(" / ");
	double tot = bytesTotal / 100000;
	tot = tot / 10.0;
	status_msg.append(QString::number(tot));
	status_msg.append(" MB");
	

	double progress = (double(bytesReceived) / double(bytesTotal)) * 100.0;
	ui_updater->progressBar_chechUpdates->setValue(progress);

	// calculate the download speed
	double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
	QString unit;
	if (speed < 1024) {
		unit = " bytes/sec";
	}
	else if (speed < 1024 * 1024) {
		speed /= 1024;
		unit = " kB/s";
	}
	else {
		speed /= 1024 * 1024;
		unit = "MB/s";
	}

	status_msg.append(" - " + m_str_speed + ": "); 
	status_msg.append(QString::number(speed));
	status_msg.append(" ");
	status_msg.append(unit);

	ui_updater->label_download_status->setText(status_msg);

}

QString biopen_updater::saveFileName(const QUrl & _url)
{

	QString path = _url.path();
	QString basename = QFileInfo(path).fileName();
	if (basename.isEmpty())
		basename = "download";

	QString save_path = QDir::tempPath();
	save_path.append(m_temp_folder);
	QDir temp_path;
	if (!temp_path.exists(save_path)) {
		//cerr << " BiopenWizard temporary directory does not exists .... creating it" << endl;
		temp_path.mkpath(save_path);
		//cout << " Created directory " <<
		    //temp_path.toStdString() << endl;
	}
	else {
		// if the temp folder exists, we can clean it for the next download
		QDir dir(save_path);
		// list all the file names
		dir.setNameFilters(QStringList() << "*.*");
		dir.setFilter(QDir::Files);
		// remove the files one by one
		foreach(QString dirFile, dir.entryList())
		{
			dir.remove(dirFile);
		}
	}

	QString fileName = save_path;
	fileName.append("/");
	fileName.append(basename);

	return fileName;
}

bool biopen_updater::saveToDisk(const QString & filename, QIODevice * data)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		ui_updater->textEdit_details->append("Could not open %s for writing: ");
		ui_updater->textEdit_details->append(qPrintable(filename));
		ui_updater->textEdit_details->append("\n");
		ui_updater->textEdit_details->append(qPrintable(file.errorString()));
		ui_updater->textEdit_details->append("\n");

		return false;
	}

	file.write(data->readAll());
	file.close();

	return true;
}

bool biopen_updater::isHttpRedirect(QNetworkReply * reply)
{
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	return statusCode == 301 || statusCode == 302 || statusCode == 303
		|| statusCode == 305 || statusCode == 307 || statusCode == 308;
}

QString biopen_updater::getHttpRedirectTarget(QNetworkReply *reply)
{
	QString target = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
	return target;
}

void biopen_updater::downloadFinished(QNetworkReply * reply)
{
	ui_updater->pushButton_cancelDownload->setEnabled(false);
	QUrl url = reply->url();
	if (reply->error()) {
		ui_updater->textEdit_details->append("Download of failed: ");
		ui_updater->textEdit_details->append(url.toEncoded().constData());
		ui_updater->textEdit_details->append("\n");
		ui_updater->textEdit_details->append(qPrintable(reply->errorString()));
		ui_updater->textEdit_details->append("\n");
	}
	else {
		if (this->isHttpRedirect(reply)) {
			if (m_verbose) {
				ui_updater->textEdit_details->append("Request was redirected to\n");
				QUrl new_url = this->getHttpRedirectTarget(reply);
				ui_updater->textEdit_details->append(new_url.toString());
			}

//THIS may be a problem as it could, in principle, generate an endless loop
/*			const QVariant redirectionTarget = 
				reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

			if (!redirectionTarget.isNull()) {
				const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
				
				doDownload(redirectedUrl);
				return;
			}*/
		}
		else
		{
			m_update_info_path = saveFileName(url);
			if (saveToDisk(m_update_info_path, reply)) {
				if (m_verbose) {
					ui_updater->textEdit_details->append("Download succeeded (saved to ");// %s)\n",
					ui_updater->textEdit_details->append(url.toEncoded().constData());
					ui_updater->textEdit_details->append(qPrintable(m_update_info_path));
					ui_updater->textEdit_details->append("\n");
				}

				if (!is_version_file_ready) {
					read_xmlinfo_file(m_update_info_path);
					is_version_file_ready = true;
					ui_updater->label_download_status->setText(" ");

					if (this->updateValidityCheck())
					{
						ui_updater->pushButton_download->setEnabled(true);

						ui_updater->label_title->setText(m_str_update_found);
						if (m_verbose) ui_updater->textEdit_details->append("Your version is outdated, please dowload the new version");
					}
					else
					{
						ui_updater->label_title->setText(m_str_no_updates);
					}
				}
				else
				{
					read_downloaded_installer(m_update_info_path);
				}
			}
		}
	}

	currentDownloads.removeAll(reply);
	reply->deleteLater();

	if (currentDownloads.isEmpty()) {
		// all downloads finished
	}
}

void biopen_updater::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
	for (const QSslError &error : sslErrors)
	{
		ui_updater->textEdit_details->append("SSL error: ");// %s\n", 
		ui_updater->textEdit_details->append(qPrintable(error.errorString()));
		ui_updater->textEdit_details->append("\n");
		QMessageBox::warning(this, m_str_warning, "SSL error: " + error.errorString());
	}

#else
	Q_UNUSED(sslErrors);
#endif
}

void biopen_updater::startUpdate()
{
	m_start_timer->stop();

	//if (this->isConnectionOk()) // if we are here, connection is already verified
	{	
		//the connection is ok, go on and download
		if (m_verbose) ui_updater->textEdit_details->append("Connection ok, downloading version file");
		this->doDownload(m_update_info_url);
	}
}

bool biopen_updater::checkConnection()
{
	if (this->isConnectionOk())
	{
		ui_updater->label_title->setText(m_str_checking);
		ui_updater->label_icon->setEnabled(true);
		m_start_timer->start();
		return true;
	}
	else 
	{
		if (m_verbose) ui_updater->textEdit_details->append(m_str_check_connection);

		ui_updater->label_icon->setEnabled(false);
		QMessageBox::warning(this, m_str_warning, m_str_check_connection);
		ui_updater->label_title->setText(m_str_no_connection);
		ui_updater->progressBar_chechUpdates->setValue(0);
		return false;
	}
}

void biopen_updater::downloadInstaller()
{
	QString system_version = QSysInfo::buildCpuArchitecture();
	int build_version = 0;

	if (m_is_experimental)
	{
		//QMessageBox::warning(this, m_str_warning, "Password is needed");
		bool ok;
		// Ask for birth date as a string.
		QString text = QInputDialog::getText(0, m_str_warning,
			"This is an experimental version, a password is required", QLineEdit::Normal,
			"", &ok);
		if (ok && !text.isEmpty()) {
			QString password = text;
			QString password_check = "FluicellGrowth2018";
			if (!password.compare(password_check))
			{
				QMessageBox::warning(this, m_str_warning, "Correct password");
				if (system_version.compare("x86_64"))
				{
					build_version = 32;
					this->doDownload(m_url_installer_32bit);
					ui_updater->pushButton_download->setEnabled(false);
				}
				else {
					this->doDownload(m_url_installer_64bit);
					build_version = 64;
					ui_updater->pushButton_download->setEnabled(false);
				}
			}
			else
			{
				QMessageBox::warning(this, m_str_warning, "Wrong password");
			}
		}
	}
}

void biopen_updater::showDetails()
{
	if (!ui_updater->pushButton_details->isChecked())
	{
		// hide details
		int gb_size_h = ui_updater->groupBox->height();
		ui_updater->groupBox->hide();
		m_details_hiden = true;
		this->resize(this->width(), this->height() - gb_size_h);
		
		//this->setFixedHeight(this->height() - gb_size_h);
		//this->setFixedWidth(this->width() - gb_size_w);
	}
	else
	{
		// show details
		ui_updater->groupBox->show();
		m_details_hiden = false;
		int gb_size_h = ui_updater->groupBox->height();
		this->resize(this->width(), this->height() + gb_size_h+120);

		//this->setFixedHeight(this->height() + gb_size_h);
		//this->setFixedWidth(this->width() + gb_size_w);
	}
}

bool biopen_updater::read_xmlinfo_file(QString _file_path)
{
	QFile xmlFile(_file_path);
	if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(this, m_str_xml_problem,
			m_str_xml_no_info, QMessageBox::Ok);
		return false;
	}
	QXmlStreamReader xml;
	xml.setDevice(&xmlFile);

	while (xml.readNextStartElement()) {
		if (xml.name() == QLatin1String("biopen")) {
			while (xml.readNextStartElement()) {
				
				if (xml.name() == QLatin1String("version"))
				{
					m_online_version = xml.readElementText();
					ui_updater->label_version->setText(QString(m_str_version + ": " + m_online_version));
					if(m_verbose) ui_updater->textEdit_details->append(m_online_version);
				}
				else if (xml.name() == QLatin1String("size"))
				{
					m_online_version_size = xml.readElementText();
					ui_updater->label_size->setText(QString(m_str_size + ": " + m_online_version_size));
					if (m_verbose) ui_updater->textEdit_details->append(m_online_version_size);

				}
				else if (xml.name() == QLatin1String("released_on"))
				{
					m_online_version_date = xml.readElementText();
					ui_updater->label_releaseDate->setText(QString(m_str_released_on + ": " + m_online_version_date));
					if (m_verbose) ui_updater->textEdit_details->append(m_online_version_date);
				}
				else if (xml.name() == QLatin1String("experimental"))
				{
					QString is_experimental = xml.readElementText();
					m_is_experimental = is_experimental.toInt();
					if (m_verbose) ui_updater->textEdit_details->append(QString::number( m_is_experimental));
				}
				else if (xml.name() == QLatin1String("link_64bit"))
				{
					m_url_installer_64bit = xml.readElementText();
					if (m_verbose) ui_updater->textEdit_details->append(m_url_installer_64bit.toString());

				}
				else if (xml.name() == QLatin1String("link_32bit"))
				{
					m_url_installer_32bit = xml.readElementText();
					if (m_verbose) ui_updater->textEdit_details->append(m_url_installer_32bit.toString());
				}
				else if (xml.name() == QLatin1String("change_list"))
				{
					ui_updater->textEdit_details->append("Change list: \n");
					while (xml.readNextStartElement()) {
						
						if (xml.name() == QLatin1String("change"))
						{
							QString ss = xml.readElementText();
							ui_updater->textEdit_details->append(ss);
							//TODO: show change list ?
						}					
					}
				}
				else {
					xml.skipCurrentElement();
				}
			}
		}
	}
	return !xml.error();

}


bool biopen_updater::read_downloaded_installer(QString _file_path)
{
	QFile file = _file_path;
	QFileInfo fileInfo(file.fileName());
	//QString filename(fileInfo.fileName());
	QString absolutePath = fileInfo.absolutePath();
	file.rename(QString(absolutePath + "/biopen_installer.exe"));
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information, m_str_success_download1 +",\n" +
			file.fileName() + " " + m_str_success_download2,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		//QMessageBox::warning(this, m_str_warning, "NOT RUN");
		// not run
	}
	else {
		QMessageBox::warning(this, m_str_warning, m_str_close_biopen);
		QDesktopServices::openUrl(QUrl("file:///" + file.fileName()));
		emit exit();
	}

	return false;
}

bool biopen_updater::compareVersions(QString _current_version, QString _online_version)
{
	QStringList current_version = _current_version.split(".");
	QStringList online_version = _online_version.split(".");

	if (current_version.size() < 2 || online_version.size() < 2) {
		return false;
	}

	// it is supposed to have the format x.y.z for example 2.1.0
	int current_version_digit = current_version.at(0).toInt();
	int online_version_digit = online_version.at(0).toInt();

	if (current_version_digit < online_version_digit) return true;

	current_version_digit = current_version.at(1).toInt();
	online_version_digit = online_version.at(1).toInt();

	if (current_version_digit < online_version_digit) return true;

	current_version_digit = current_version.at(2).toInt();
	online_version_digit = online_version.at(2).toInt();
	if (current_version_digit < online_version_digit) return true;

	// if non of them is true, it will return false
	return false;
}

bool biopen_updater::updateValidityCheck()
{

	m_is_update_available = this->compareVersions(m_current_version, m_online_version);

	if (!m_is_update_available) return false;

	if (!m_is_window_active) {
		if (m_notify_experimental_ver)
		{
			emit updateAvailable();
		}
		else
		{
			if (m_is_experimental == 0)
			{
				emit updateAvailable();
			}
		}
	}


	if (!m_url_installer_64bit.isValid())
	{
		ui_updater->textEdit_details->append("the installer 64 bit is not a valid address");
		QMessageBox::warning(this, m_str_warning, m_str_not_valid_url);
		return false;
	}

	if (!m_url_installer_32bit.isValid())
	{
		ui_updater->textEdit_details->append("the installer 32 bit is not a valid address");
		QMessageBox::warning(this, m_str_warning, m_str_not_valid_url);
		return false;
	}
	
	return true;
}



void biopen_updater::initCustomStrings( )
{
	m_str_warning = tr("Warning");
	m_str_areyousure = tr("Are you sure?");
	m_str_information = tr("Information");
	m_str_ok = tr("Ok");
	m_str_no_connection = tr("Please, connect your device to internet to check for updates");
	m_str_checking = tr("Connection ok, checking for online updates");
	m_str_checking_connection = tr("Checking connection");
	m_str_update_found = tr("Newer version available");
	m_str_no_updates = tr("Your version is up-to-date");
	m_str_downloaded = tr("Downloaded");
	m_str_speed = tr("speed");
	m_str_file_not_found = tr("file not found");
	m_str_success_download1 = tr("The installer was downloaded at");
	m_str_success_download2 = tr("do you want to run the instaler now?");
	m_str_close_biopen = tr("Biopen wizard will be closed for the update");
	m_str_check_connection = tr("Biopen wizard could not reach fluicell.com, check your internet connection");
	m_str_version = tr("Version");
	m_str_size = tr("Size");
	m_str_released_on = tr("Released on");
	m_str_not_valid_url = tr("Not valid url");
	m_str_download_cancelled = tr("Download cancelled");
	m_str_xml_problem = tr("Load XML File Problem");
	m_str_xml_no_info = tr("Could not open the xml file to load settings for download");
}

void biopen_updater::switchLanguage(QString _translation_file)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "biopen_updater::switchLanguage " << endl;

	qApp->removeTranslator(&m_translator_bu);

	if (m_translator_bu.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_bu);

		ui_updater->retranslateUi(this);

		initCustomStrings();

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "biopen_updater::switchLanguage   installTranslator" << endl;
	}

}

void biopen_updater::closeEvent(QCloseEvent *_event) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "biopen_updater::closeEvent   " << endl;

	m_is_window_active = false;
	return;

	// the message is not shown
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information, m_str_areyousure,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {	
		
		//_event->ignore();
	}
	else {

		//_event->accept();
	}
}


biopen_updater::~biopen_updater() {

	// TODO: clean downloaded files (but not if the installer run) 
	cleanTempFolder();
	delete ui_updater;
}
