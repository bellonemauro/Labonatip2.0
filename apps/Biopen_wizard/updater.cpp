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

biopen_updater::biopen_updater(QWidget *parent):
	QMainWindow(parent),
	ui_updater(new Ui::Updater)
{
	//Main things to add: 
	// 1. trigger the exit event to avoid pending download
	// 2. the cancel button is still not wired
		ui_updater->setupUi(this );
		this->initCustomStrings();

		m_update_info_path = "";
		m_current_version = "";
		m_online_version = "";
		m_online_version_size = "";
		m_online_version_date = "";
		m_details_hiden = false;

		m_start_timer = new QTimer();
		m_start_timer->setInterval(100);
		m_url_installer_64bit = "";
		m_url_installer_32bit = "";
		is_version_file_ready = false;

		ui_updater->pushButton_download->setEnabled(false);

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
	ui_updater->label_title->setText(m_str_checking); 
	
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
	// this timer is required to start the actual online request
	m_start_timer->start();
}


bool biopen_updater::isConnectionOk()
{

	QNetworkAccessManager nam;
	QNetworkRequest req(QUrl("http://fluicell.com/"));
	QNetworkReply *reply = nam.get(req);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	if (reply->bytesAvailable())
		return true;
	else
		return false;
}


void biopen_updater::doDownload(const QUrl & _url)
{
	QNetworkRequest request(_url);
	QNetworkReply *reply = manager.get(request);

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
	save_path.append("/biopen_tmp"); // temporary folder for download
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
	double br = bytesReceived / 100000;
	br = br / 10.0;
	status_msg.append(QString::number(br));
	status_msg.append(" / ");
	double tot = bytesTotal / 100000;
	tot = br / 10.0;
	status_msg.append(QString::number(tot));
	status_msg.append(" MB");
	

	double progress = (double(bytesReceived) / double(bytesTotal)) * 100.0;
	ui_updater->progressBar_chechUpdates->setValue(progress);

	// calculate the download speed
	double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
	QString unit;
	if (speed < 1024) {
		unit = "bytes/sec";
	}
	else if (speed < 1024 * 1024) {
		speed /= 1024;
		unit = "kB/s";
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

	//progressBar.setMessage(QString::fromLatin1("%1 %2")
	//	.arg(speed, 3, 'f', 1).arg(unit));
	//progressBar.update();
}

QString biopen_updater::saveFileName(const QUrl & _url)
{

	QString path = _url.path();
	QString basename = QFileInfo(path).fileName();
	if (basename.isEmpty())
		basename = "download";

	QString save_path = QDir::tempPath();
	save_path.append("/biopen_tmp"); // temporary folder for download
	QDir temp_path;
	if (!temp_path.exists(save_path)) {
		//cerr << " BiopenWizard temporary directory does not exists .... creating it" << endl;
		temp_path.mkpath(save_path);
		//cout << " Created directory " <<
		    //temp_path.toStdString() << endl;
	}
	else {
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
			ui_updater->textEdit_details->append("Request was redirected to\n");
			url = this->getHttpRedirectTarget(reply);
			ui_updater->textEdit_details->append(url.toString());
		}
		else
		{
			m_update_info_path = saveFileName(url);
			if (saveToDisk(m_update_info_path, reply)) {
				ui_updater->textEdit_details->append("Download of succeeded (saved to ");// %s)\n",
				ui_updater->textEdit_details->append(url.toEncoded().constData());
				ui_updater->textEdit_details->append(qPrintable(m_update_info_path));
				ui_updater->textEdit_details->append("\n");

				if (!is_version_file_ready) {
					read_info_file(m_update_info_path);
					is_version_file_ready = true;
					ui_updater->label_download_status->setText(" ");
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
		//QCoreApplication::instance()->quit();
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

	if (this->isConnectionOk())
	{
		//the connection is ok, go on and download
		ui_updater->textEdit_details->append("Connection ok, downloading version file");
		this->doDownload(QUrl("https://raw.githubusercontent.com/bellonemauro/Labonatip2.0/master/BpLRMB.txt"));
	}
	else
	{
		ui_updater->textEdit_details->append(m_str_check_connection);
		ui_updater->textEdit_details->append(m_str_check_connection);
	}

}

void biopen_updater::downloadInstaller()
{
	QString system_version = QSysInfo::buildCpuArchitecture();
	int build_version = 0;
	if (system_version.compare("x86_64"))
	{
		this->doDownload(m_url_installer_64bit);
		build_version = 64;
		ui_updater->pushButton_download->setEnabled(false);
	}
	else {
		build_version = 32;
		this->doDownload(m_url_installer_32bit);
		ui_updater->pushButton_download->setEnabled(false);
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

bool biopen_updater::read_info_file(QString _file_path)
{
	QFile info_file(_file_path);

	if (info_file.exists() &&
		info_file.open(QIODevice::ReadWrite))
	{
		// this is the protocol file content
		QByteArray content = info_file.readLine();

		ui_updater->textEdit_details->append("The current version is : ");
		ui_updater->textEdit_details->append(m_current_version);
		ui_updater->textEdit_details->append("\n");
		ui_updater->textEdit_details->append("System info : "); 
		QString system_version = QSysInfo::buildCpuArchitecture();
		int build_version = 0;
		if (system_version.compare("x86_64"))
			build_version = 64;
		else
			build_version = 32;
		ui_updater->textEdit_details->append(system_version);
		ui_updater->textEdit_details->append("\n");

		// browse the entire file content
		while (!content.isEmpty())
		{
			// the first line is the version
			m_online_version = content; 
			content = content.remove(content.size() - 1, 2); //I need to remove the last character to avoid "\n" to be considered
			ui_updater->label_version->setText(QString(m_str_version + ": " + content));

			// the second line is the size of the installer
			content.clear();
			content = info_file.readLine(); 
			content = content.remove(content.size()-1, 2);
			ui_updater->label_size->setText(QString(m_str_size + ": " + content));

			// the third line is the date of release
			content.clear();
			content = info_file.readLine();
			content = content.remove(content.size() - 1, 2);
			ui_updater->label_releaseDate->setText(QString(m_str_released_on + ": " + content));

			// the forth line is the url of the installer in 64 bit
			content.clear();
			content = info_file.readLine();
			m_url_installer_64bit = content;

			// the fifth line is the url of the installer in 32 bit
			content.clear();
			content = info_file.readLine();		
			m_url_installer_32bit = content;

			content.clear();
			// we are ready to read the new line
			content = info_file.readLine();
		}

		ui_updater->textEdit_details->append("The online version is: ");
		ui_updater->textEdit_details->append(m_online_version);
		ui_updater->textEdit_details->append("\n");
		ui_updater->textEdit_details->append("The 64bit installer url is:");
		ui_updater->textEdit_details->append(m_url_installer_64bit.toString());
		ui_updater->textEdit_details->append("\n");
		ui_updater->textEdit_details->append("The 32bit installer url is:");
		ui_updater->textEdit_details->append(m_url_installer_32bit.toString());

		bool sf_version_success = false;
		sf_version_success = this->compareVersions(m_current_version, m_online_version);

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

		if (sf_version_success)
		{
			ui_updater->pushButton_download->setEnabled(true);
			
			ui_updater->label_title->setText(m_str_update_found);
			ui_updater->textEdit_details->append("Your version is outdated, please dowload the new version");
		}
		else
		{
			ui_updater->label_title->setText(m_str_no_updates);
		}

	}
	else
	{
		QMessageBox::warning(this,m_str_warning, m_str_file_not_found);
		return false;
	}
	return true;

}

bool biopen_updater::read_downloaded_installer(QString _file_path)
{

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information, m_str_success_download1 +",\n" +
			_file_path + " " + m_str_success_download1,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		//QMessageBox::warning(this, m_str_warning, "NOT RUN");
		// not run
	}
	else {
		QMessageBox::warning(this, m_str_warning, m_str_close_biopen);
		QDesktopServices::openUrl(QUrl("file:///" + _file_path));
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



void biopen_updater::initCustomStrings( )
{
	m_str_warning = tr("Warning");
	m_str_areyousure = tr("Are you sure?");
	m_str_information = tr("Information");
	m_str_ok = tr("Ok");
	m_str_checking = tr("Checking for online updates, please wait");
	m_str_update_found = tr("Newer version available");
	m_str_no_updates = tr("Your version is up-to-date");
	m_str_downloaded = tr("Downloaded");
	m_str_speed = tr("speed");
	m_str_file_not_found = tr("file not found");
	m_str_success_download1 = tr("The installer was downloaded at");
	m_str_success_download2 = tr("do you want to run the instaler now?");
	m_str_close_biopen = tr("Biopen wizard will be now closed for the update");
	m_str_check_connection = tr("Biopen wizard could not reach fluicell.com, check your internet connection");
	m_str_version = tr("Version");
	m_str_size = tr("Size");
	m_str_released_on = tr("Released on");
	m_str_not_valid_url = tr("Not valid url");
	m_str_download_cancelled = tr("Download cancelled");
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

	return;
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
