/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"


void Labonatip_GUI::openSettingsFile() {  // open setting file

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::openFile    " << endl;

	QString _path = QFileDialog::getOpenFileName(this, m_str_load_profile, m_settings_path,  // dialog to open files
		"Profile file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) {
        QMessageBox::information(this, m_str_information, m_str_no_file_loaded + "<br>" + _path);
		return;
	}

	if (!m_dialog_tools->setLoadSettingsFileName(_path)) {
		QMessageBox::warning(this, m_str_warning, m_str_cannot_load_profile + "<br>" + _path);
		return;
	}

	toolApply();
}

void Labonatip_GUI::saveSettingsFile() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::saveFile    " << endl;

	QString _path = QFileDialog::getSaveFileName(this, m_str_save_profile, m_settings_path,  // dialog to open files
		"Profile file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) { 
	    QMessageBox::information(this, m_str_information, m_str_cannot_save_profile + "<br>" + _path);
		return;
	}

	if (!m_dialog_tools->setFileNameAndSaveSettings(_path)) {
		QMessageBox::warning(this, m_str_warning, m_str_cannot_save_profile + "<br>" + _path);
		return;
	}
}


void Labonatip_GUI::showToolsDialog() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showToolsDialog    " << endl;

	m_dialog_tools->setParent(this);
	m_dialog_tools->setWindowFlags(Qt::Window);
	m_dialog_tools->show();
	m_dialog_tools->setExtDataPath(m_ext_data_path); //TODO: this force the file path in the tools
}


void Labonatip_GUI::showProtocolEditorDialog() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showProtocolEditorDialog    " << endl;

	m_dialog_p_editor->setMacroPath(m_protocol_path); //TODO: reset the path in case it is changed in the settings
	m_dialog_p_editor->setPrParams(*m_pr_params);
	m_dialog_p_editor->setSolParams(*m_solutionParams);

	m_dialog_p_editor->setParent(this);
	m_dialog_p_editor->setWindowFlags(Qt::Window);
	//m_dialog_p_editor->setModal(true);
	m_dialog_p_editor->setMacroPrt(m_protocol);   //TODO: this is wrong, two classes act on the same memory location
	m_dialog_p_editor->show();
}


void Labonatip_GUI::simulationOnly()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::simulationOnly    " << ui->actionSimulation->isChecked() << endl;

	m_simulationOnly = ui->actionSimulation->isChecked();

	ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
	ui->actionReboot->setEnabled(!m_simulationOnly);
	ui->actionShudown->setEnabled(!m_simulationOnly);
	m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
}

/*
void Labonatip_GUI::disCon() {   //TODO, add an argument to connect and disconnect

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::disCon    " << endl;

	//TODO: this function is deprecated, remove the possible references
	disCon(ui->actionConnectDisconnect->isChecked());

}*/


void Labonatip_GUI::shutdown() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::shutdown   " << endl;

	setEnableMainWindow(false);
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, m_str_warning,
		m_str_shutdown_pressed,
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
		if (!visualizeProgressMessage(10, 
			m_str_shutdown_pressed_p_off)) return;
		
		updateVrecircSetPoint(0.0);
		updateVswitchSetPoint(0.0);

		//Wait 15 seconds
		if (!visualizeProgressMessage(15, 
			m_str_shutdown_pressed_v_off)) return;
		
	}
	else {
		// do nothing for now
	}
	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}


bool Labonatip_GUI::disCon(bool _connect)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::disCon    " << endl;



	if (m_simulationOnly) {
		QMessageBox::information(this, m_str_warning, m_str_warning_simulation_only);
		return false;
	}

	try
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

		if (_connect) {
		    // connect 
			if (!m_ppc1->isRunning() && !m_ppc1->isConnected() ) { // if not running and connected already
				// we try to connect
				if (!m_ppc1->connectCOM()) {  
					// if the connection is NOT success
					this->setStatusLed(false);
					ui->status_PPC1_label->setText(m_str_PPC1_status_discon);
					ui->actionConnectDisconnect->setText(m_str_connect);
					ui->actionSimulation->setEnabled(true);
					QApplication::restoreOverrideCursor();   
					QMessageBox::information(this, m_str_warning,
						QString(m_str_cannot_connect_ppc1 + "\n" + m_str_cannot_connect_ppc1_check_cables));

					// ask for a new attempt to connect
					QMessageBox::StandardButton resBtn =
						QMessageBox::question(this, m_str_information,
							m_str_question_find_device,
							QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
							QMessageBox::Yes);
					if (resBtn != QMessageBox::Yes) {  // if the answer is not YES
						m_pipette_active = false;
						ui->actionConnectDisconnect->setChecked(false);
						return false;  // the device is not connected
					}
					else {  // new attempt to connect 
						m_dialog_tools->updateDevices();
						m_ppc1->setCOMport(m_comSettings->getName());
						m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());
						if (!m_ppc1->connectCOM())
						{
							QMessageBox::information(this, m_str_warning,
								QString(m_str_cannot_connect_ppc1_twice + "\n" + m_str_cannot_connect_ppc1_check_cables));
							m_pipette_active = false;
							ui->actionConnectDisconnect->setChecked(false);
							return false;  // we could not connect twice
						}
					}
				}

				// if we are here the PPC1 is connected 
				QThread::msleep(250);  // just to be sure that the device is properly running

				// run the ppc1 thread
				m_ppc1->run();   // TODO: this is not the best way of running the device as it cannot handle exeptions
				QThread::msleep(250);

				// test if we are running properly
				if (m_ppc1->isRunning()) {  // if running, everything is fine
					m_pipette_active = true;
					ui->actionConnectDisconnect->setChecked(true);
					m_update_GUI->start();
					this->setStatusLed(true);
					ui->status_PPC1_label->setText(m_str_PPC1_status_con);
					ui->actionConnectDisconnect->setText(m_str_disconnect);
					ui->actionSimulation->setEnabled(false);
					QApplication::restoreOverrideCursor();
					return true; // we are finally connected
				}
				else {  // otherwise something is wrong
					QMessageBox::information(this, m_str_warning,
						m_str_ppc1_connected_but_not_running);
					
					m_ppc1->stop();
					m_ppc1->disconnectCOM();
					m_pipette_active = false;
					this->setStatusLed(false);
					ui->status_PPC1_label->setText(m_str_PPC1_status_discon);
					ui->actionConnectDisconnect->setText(m_str_connect);
					ui->actionSimulation->setEnabled(true);
					ui->actionConnectDisconnect->setChecked(false);
					QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
					return false;
				}

			}
		} // end connect
		else {
			// disconnect 
			QApplication::restoreOverrideCursor();
			// the user should confirm to stop the device
			QMessageBox::StandardButton resBtn =
				QMessageBox::question(this, m_str_warning,
					QString(m_str_question_stop_ppc1 + "\n" + m_str_areyousure),
					QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
					QMessageBox::Yes);
			if (resBtn != QMessageBox::Yes) {  // if the answer is not YES
				//TODO: check !!!
				ui->actionConnectDisconnect->setChecked(true);
				return false; // it should be false, but in this case is a user choice? 
			}

			if (!m_ppc1->isRunning()) return true; // already stop

			// stop the PPC1
			QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
			m_ppc1->stop();
			QThread::msleep(500);
			if (m_ppc1->isConnected())
				m_ppc1->disconnectCOM();
			QThread::msleep(500);

			if (!m_ppc1->isRunning()) { // verify that it really stopped
				this->setStatusLed(false);
				ui->status_PPC1_label->setText(m_str_PPC1_status_discon);
				ui->actionConnectDisconnect->setText(m_str_connect);
				m_pipette_active = false;
				ui->actionSimulation->setEnabled(true);
				QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
				return true;
			}
			else {  // the device was not stopped
				ui->actionConnectDisconnect->setChecked(false);
				m_update_GUI->stop();
				this->setStatusLed(true);
				ui->status_PPC1_label->setText(m_str_PPC1_status_con);
				ui->actionConnectDisconnect->setText(m_str_disconnect);
				ui->actionSimulation->setEnabled(false);
				QMessageBox::information(this, m_str_warning,
					m_str_unable_stop_ppc1);
				QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
				return false;   
			}
		} // end disconnect
	} // end try
	catch (serial::IOException &e) 	{
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: IOException : " << e.what() << endl;
		//m_PPC1_serial->close();
		return false;
	}
	catch (serial::PortNotOpenedException &e)	{
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: PortNotOpenedException : " << e.what() << endl;
		//m_PPC1_serial->close();
		return false;
	}
	catch (serial::SerialException &e)	{
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: SerialException : " << e.what() << endl;
		//m_PPC1_serial->close();
		return false;
	}
	catch (exception &e) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: Unhandled Exception: " << e.what() << endl;
		//m_PPC1_serial->close();
		return false;
	}
	return false;
}

void Labonatip_GUI::reboot() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::reboot    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	setEnableMainWindow(false);

	if (m_pipette_active) {
		disCon(false); // with the pipette active this will stop the threads

		if (m_pipette_active) { // if it is still active, the disconnection failed and and we cannot continue
			setEnableMainWindow(true);
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			return;
		}

		ui->actionSimulation->setChecked(false);
	
		m_ppc1->reboot();

		updatePonSetPoint(0.0);
		updatePoffSetPoint(0.0);
		updateVrecircSetPoint(0.0);
		updateVswitchSetPoint(0.0);

		if (!visualizeProgressMessage(200, m_str_rebooting)) return;

		m_ppc1->connectCOM();

		if (!visualizeProgressMessage(5, m_str_reconnecting)) return;

		// TODO : check if connected and fine 
		// m_ppc1->isConnected();
		// m_ppc1->isRunning();

		disCon(true); //TODO: check this, it is not clear if we connect or disconnect
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
		ui->actionAdvanced->setText(m_str_advanced);
		m_g_spacer->setFixedWidth(1);
		ui->toolBar_2->update(); 
		ui->toolBar_3->update();
		if (!this->isMaximized())
			this->resize(QSize(this->width(), this->height()));
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
		ui->actionAdvanced->setText(m_str_basic);
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