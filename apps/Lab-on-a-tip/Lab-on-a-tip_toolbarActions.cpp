/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"


void Labonatip_GUI::openFile() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::openFile    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QString _path = QFileDialog::getOpenFileName(this, tr("Open Settings file"), m_settings_path,  // dialog to open files
		"Settings file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

    QMessageBox::information(this, "Information !", "No file loaded ! <br>" + _path);
		return;
	}

	if (!m_dialog_tools->setLoadSettingsFileName(_path)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "Cannot load the file ! <br>" + _path);
		return;
	}

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::saveFile() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::saveFile    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	
	QString _path = QFileDialog::getSaveFileName(this, tr("Save configuration file"), m_settings_path,  // dialog to open files
		"Settings file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
												  
	QMessageBox::information(this, "Information !", "No file saved ! <br>" + _path);
	
		return;
	}

	if (!m_dialog_tools->setSaveSettingsFileName(_path)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "Cannot save the file ! <br>" + _path);
		return;
	}

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}



void Labonatip_GUI::showToolsDialog() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showToolsDialog    " << endl;

	m_dialog_tools->setMacroPath(m_macro_path); //TODO: reset the macro path in case it is changed in the settings

	m_dialog_tools->setWindowFlags(Qt::WindowFullscreenButtonHint);
	m_dialog_tools->setModal(false);
	m_macro = new std::vector<fluicell::PPC1api::command>();
	m_dialog_tools->setMacroPrt(m_macro);
	m_dialog_tools->show();

}


void Labonatip_GUI::simulationOnly()
{
	m_simulationOnly = ui->actionSimulation->isChecked();

	ui->actionDisCon->setEnabled(!m_simulationOnly);
	ui->actionRun->setEnabled(!m_simulationOnly);
	ui->actionReset->setEnabled(!m_simulationOnly);
	ui->actionSleep->setEnabled(!m_simulationOnly);
	ui->actionShutdown->setEnabled(!m_simulationOnly);
	m_macroRunner_thread->setSimulationFlag(m_simulationOnly);

	if (m_simulationOnly)ui->treeWidget_macroInfo->topLevelItem(0)->setText(1, "Simulation");
	else ui->treeWidget_macroInfo->topLevelItem(0)->setText(1, "PPC1");

}


void Labonatip_GUI::disCon() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::disCon    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	if (m_simulationOnly) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::information(this, "Warning !", "Lab-on-a-tip is in simulation only  ");
		return;
	}

	try
	{

		if (!m_ppc1->isRunning()) {

			if (!m_ppc1->isConnected())
				if (!m_ppc1->connectCOM()) {
					ui->statusBar->showMessage("STATUS: NOT Connected  ");
					ui->actionDisCon->setIconText("Connect");
					ui->actionSimulation->setEnabled(true);
					QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
					QMessageBox::information(this, "Warning !",
						"Lab-on-a-tip could not connect to PPC1, \n please check the cable and settings  ");
					m_pipette_active = false;
					ui->actionDisCon->setChecked(false);
					return;
				}
			QThread::msleep(500);

			m_ppc1->run();   // TODO: this is not the best way of running the device as it cannot handle exeptions
			QThread::msleep(500);
			if (m_ppc1->isRunning()) {
				m_pipette_active = true;
				ui->actionDisCon->setChecked(true);
				m_update_GUI->start();
				ui->statusBar->showMessage("STATUS: Connected  ");
				ui->actionDisCon->setIconText("Disconnect");
				ui->actionSimulation->setEnabled(false);
			}
			else {
				QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
				QMessageBox::information(this, "Warning !",
					"Lab-on-a-tip connected but not running on PPC1 ");
				m_ppc1->stop();
				m_ppc1->disconnectCOM();

				m_pipette_active = false;
				ui->statusBar->showMessage("STATUS: NOT Connected  ");
				ui->actionDisCon->setIconText("Connect");
				ui->actionSimulation->setEnabled(true);
				ui->actionDisCon->setChecked(false);
				return;
			}
		} // if m_ppc1 not running 
		else
		{
			m_ppc1->stop();
			QThread::msleep(500);
			if (m_ppc1->isConnected())
				m_ppc1->disconnectCOM();
			QThread::msleep(500);

			if (!m_ppc1->isRunning()) { // verify that it really stopped
				ui->statusBar->showMessage("STATUS: NOT Connected  ");
				ui->actionDisCon->setIconText("Connect");
				m_pipette_active = false;
				ui->actionSimulation->setEnabled(true);
			}
			else {
				ui->actionDisCon->setChecked(false);
				m_update_GUI->stop();
				ui->statusBar->showMessage("STATUS: Connected  ");
				ui->actionDisCon->setIconText("Disconnect");
				ui->actionSimulation->setEnabled(false);
				QMessageBox::information(this, "Warning !",
					"Unable to stop and disconnect ");
				QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
				return;
			}
		}

	}
	catch (serial::IOException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: IOException : " << e.what() << endl;
		//m_PPC1_serial->close();
		return;
	}
	catch (serial::PortNotOpenedException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: PortNotOpenedException : " << e.what() << endl;
		//m_PPC1_serial->close();
		return;
	}
	catch (serial::SerialException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: SerialException : " << e.what() << endl;
		//m_PPC1_serial->close();
		return;
	}
	catch (exception &e) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: Unhandled Exception: " << e.what() << endl;
		//m_PPC1_serial->close();
		return;
	}


	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}


void Labonatip_GUI::shutdown() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::shutdown   " << endl;

	setEnableMainWindow(false);
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Lab-on-a-tip",
		tr("shutdown pressed, this will take 30 seconds, press ok to continue, cancel to abort. \n"),
		QMessageBox::Cancel | QMessageBox::Ok,
		QMessageBox::Ok);
	if (resBtn != QMessageBox::Cancel) {

		//RUN THE FOLLOWING MACRO:
		//	allOff()
		//	setPon(0)
		//	setPoff(0)
		//	sleep(10)
		//	setVswitch(0)
		//	setVrecirc(0)
		//	sleep(15)
		//	pumpsOff()
		QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}
		updatePonSetPoint(0.0);
		updatePoffSetPoint(0.0);
		
		//Wait 10 seconds
		if (!visualizeProgressMessage(10, " The pressure is off, waiting for the vacuum. ")) return;
		
		updateVrecircSetPoint(0.0);
		updateVswitchSetPoint(0.0);

		//Wait 15 seconds
		if (!visualizeProgressMessage(15, " Vacuum off. Stopping the flow in the device. ")) return; 
		
	}
	else {
		// do nothing for now
	}
	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}




void Labonatip_GUI::reboot() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::reboot    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	setEnableMainWindow(false);

	if (m_pipette_active) {
		disCon(); // with the pipette active this will stop the threads
		ui->actionSimulation->setChecked(false);
	
		m_ppc1->reboot();

		updatePonSetPoint(0.0);
		updatePoffSetPoint(0.0);
		updateVrecircSetPoint(0.0);
		updateVswitchSetPoint(0.0);

		if (!visualizeProgressMessage(200, " Rebooting ... ")) return;

		m_ppc1->connectCOM();

		if (!visualizeProgressMessage(5, " Reconnecting ... ")) return;

		// TODO : check if connected and fine 
		// m_ppc1->isConnected();
		// m_ppc1->isRunning();

		disCon();
	}

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}


void Labonatip_GUI::closeOpenDockTools() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::closeOpenDockTools   " << endl;

	// get the screen resolution of the current screen
	// so we can resize the application in case of small screens
	//QRect rec = QApplication::desktop()->screenGeometry();
	//int screen_height = rec.height();
	//int screen_width = rec.width();

	if (!ui->dockWidget->isHidden()) {
		ui->dockWidget->hide();
		if (!this->isMaximized())
			this->resize(QSize(this->width(), this->height()));//	this->resize(QSize(this->minimumWidth(), this->height()));
	}
	else {

		if (this->width() < this->minimumWidth() + ui->dockWidget->width())
		{
			ui->dockWidget->show();
			if (!this->isMaximized())
				this->resize(QSize(this->width() + ui->dockWidget->width(), this->height()));
		}
		else
		{
			ui->dockWidget->show();
			if (!this->isMaximized())
				this->resize(QSize(this->width(), this->height()));
		}

	}

}