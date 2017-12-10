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

    QMessageBox::information(this, "Information ", "No file loaded ! <br>" + _path);
		return;
	}

	if (!m_dialog_tools->setLoadSettingsFileName(_path)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning ", "Cannot load the file ! <br>" + _path);
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
												  
	QMessageBox::information(this, "Information ", "No file saved ! <br>" + _path);
	
		return;
	}

	if (!m_dialog_tools->setSaveSettingsFileName(_path)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning ", "Cannot save the file ! <br>" + _path);
		return;
	}

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}



void Labonatip_GUI::showToolsDialog() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showToolsDialog    " << endl;

	//m_dialog_tools->setMacroPath(m_macro_path); //TODO: reset the macro path in case it is changed in the settings

	//m_dialog_tools->setWindowFlags(Qt::WindowFullscreenButtonHint);
	//m_dialog_tools->setWindowFlags(Qt::WindowStaysOnTopHint);
	m_dialog_tools->setParent(this);
	m_dialog_tools->setWindowFlags(Qt::Window);
//	m_dialog_tools->setModal(true);
	m_macro = new std::vector<fluicell::PPC1api::command>();
	//m_dialog_tools->setMacroPrt(m_macro);
	m_dialog_tools->show();

}

void Labonatip_GUI::showProtocolEditorDialog() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showProtocolEditorDialog    " << endl;

	m_dialog_p_editor->setMacroPath(m_macro_path); //TODO: reset the macro path in case it is changed in the settings

												//m_dialog_tools->setWindowFlags(Qt::WindowFullscreenButtonHint);
												//m_dialog_tools->setWindowFlags(Qt::WindowStaysOnTopHint);
	m_dialog_p_editor->setParent(this);
	m_dialog_p_editor->setWindowFlags(Qt::Window);
	m_dialog_p_editor->setModal(true);
	m_macro = new std::vector<fluicell::PPC1api::command>();
	m_dialog_p_editor->setMacroPrt(m_macro);
	m_dialog_p_editor->show();

}

void Labonatip_GUI::simulationOnly()
{
	m_simulationOnly = ui->actionSimulation->isChecked();

	ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
	//ui->actionRun->setEnabled(!m_simulationOnly);
	ui->actionReboot->setEnabled(!m_simulationOnly);
	ui->actionShudown->setEnabled(!m_simulationOnly);
	//ui->actionShutdown->setEnabled(!m_simulationOnly);
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
		QMessageBox::information(this, "Warning ", "Lab-on-a-tip is in simulation only  ");
		return;
	}

	try
	{

		if (!m_ppc1->isRunning()) {

			if (!m_ppc1->isConnected())
				if (!m_ppc1->connectCOM()) {
					//ui->statusBar->showMessage("STATUS: NOT Connected  ");
					this->setStatusLed(false);
					ui->status_PPC1_label->setText("PPC1 STATUS: NOT Connected  ");
					ui->actionConnectDisconnect->setText("Connect");
					ui->actionSimulation->setEnabled(true);
					QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
					QMessageBox::information(this, "Warning ",
						"Lab-on-a-tip could not connect to PPC1, \n please check cables and settings and press ok ");

					QMessageBox::StandardButton resBtn =
						QMessageBox::question(this, "Lab-on-a-tip",
							tr("I can try to automatically find the device, \n Should I do it?\n"),
							QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
							QMessageBox::Yes);
					if (resBtn != QMessageBox::Yes) {
						m_pipette_active = false;
						ui->actionConnectDisconnect->setChecked(false);
						return;
					}
					else {
						m_dialog_tools->updateDevices();
						m_ppc1->setCOMport(m_comSettings->getName());
						m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());
						if (!m_ppc1->connectCOM())
						{
							QMessageBox::information(this, "Warning ",
								"Lab-on-a-tip could not connect to PPC1 twice, \n please check cables and settings  ");
							m_pipette_active = false;
							ui->actionConnectDisconnect->setChecked(false);
							return;
						}
					}


				}
			QThread::msleep(500);

			m_ppc1->run();   // TODO: this is not the best way of running the device as it cannot handle exeptions
			QThread::msleep(500);
			if (m_ppc1->isRunning()) {
				m_pipette_active = true;
				ui->actionConnectDisconnect->setChecked(true);
				m_update_GUI->start();
				//ui->statusBar->showMessage("STATUS: Connected  ");
				this->setStatusLed(true);
				ui->status_PPC1_label->setText("PPC1 STATUS: Connected  ");
				ui->actionConnectDisconnect->setText("Disconnect");
				ui->actionSimulation->setEnabled(false);
			}
			else {
				QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
				QMessageBox::information(this, "Warning ",
					"Lab-on-a-tip connected but not running on PPC1 ");
				m_ppc1->stop();
				m_ppc1->disconnectCOM();

				m_pipette_active = false;
				//ui->statusBar->showMessage("STATUS: NOT Connected  ");
				this->setStatusLed(false);
				ui->status_PPC1_label->setText("PPC1 STATUS: NOT Connected  ");
				ui->actionConnectDisconnect->setText("Connect");
				ui->actionSimulation->setEnabled(true);
				ui->actionConnectDisconnect->setChecked(false);
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
				//ui->statusBar->showMessage("STATUS: NOT Connected  ");
				this->setStatusLed(false);
				ui->status_PPC1_label->setText("PPC1 STATUS: NOT Connected  ");
				ui->actionConnectDisconnect->setText("Connect");
				m_pipette_active = false;
				ui->actionSimulation->setEnabled(true);
			}
			else {
				ui->actionConnectDisconnect->setChecked(false);
				m_update_GUI->stop();
				//ui->statusBar->showMessage("STATUS: Connected  ");
				this->setStatusLed(true);
				ui->status_PPC1_label->setText("PPC1 STATUS: Connected  ");
				ui->actionConnectDisconnect->setText("Disconnect");
				ui->actionSimulation->setEnabled(false);
				QMessageBox::information(this, "Warning ",
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
		tr("Shutdown pressed, this will take 30 seconds, press ok to continue, cancel to abort. \n"),
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


	if (!ui->dockWidget->isHidden()) {
		ui->dockWidget->hide();
		ui->toolBar_2->removeAction(m_a_spacer);
		m_g_spacer = new QGroupBox();
		m_a_spacer = new QAction();
		ui->actionAdvanced->setText("Advanced");
		m_g_spacer->setFixedWidth(1);
		ui->toolBar_2->update(); 
		ui->toolBar_3->update();
		if (!this->isMaximized())
			this->resize(QSize(this->width(), this->height()));//	this->resize(QSize(this->minimumWidth(), this->height()));
	}
	else {
		
		if (this->width() < this->minimumWidth() + ui->dockWidget->width())
		{
			if (!this->isMaximized())
				this->resize(QSize(this->width() + ui->dockWidget->width(), this->height()));

			ui->dockWidget->show();
		}
		else
		{
			ui->dockWidget->show();
			if (!this->isMaximized())
				this->resize(QSize(this->width(), this->height()));
		}

		QRect rec = this->geometry();
		int app_height = rec.height();
		int app_width = rec.width();


		//TODO: this is a really shitty method
		int spacer = ui->toolBar->width() +
			ui->toolBar_2->width() +
			ui->toolBar_2->iconSize().width() + 12; //12px is to avoid the widget to go to a new line

		m_g_spacer->setFixedWidth(app_width - spacer);
		m_g_spacer->setStyleSheet("border:0;");
		m_g_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		m_a_spacer = ui->toolBar_2->addWidget(m_g_spacer);

		ui->actionAdvanced->setText("Basic");
		ui->toolBar_2->update(); 
		ui->toolBar_3->update();
	}

}


void Labonatip_GUI::resizeToolbar()
{

	if (!ui->actionAdvanced->isChecked())return;  // it does nothing if the advanced tab is closed

		QRect rec_app = this->geometry();
		int app_height = rec_app.height();
		int app_width = rec_app.width();

		//TODO: this is a really shitty method
		int spacer = ui->toolBar->width() +
				10 * ui->toolBar_2->iconSize().width(); //10 is the number of icons 
	
		m_g_spacer->setFixedWidth(app_width - spacer);

		ui->toolBar_3->update();
		ui->toolBar_2->update();

}