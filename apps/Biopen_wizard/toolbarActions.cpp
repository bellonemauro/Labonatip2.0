/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"

void Labonatip_GUI::loadPressed()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::loadPressed   " << endl;

	if (ui->stackedWidget_main->currentIndex() == 0)
	{
		openSettingsFile();
	}
	else
	{
		loadProtocol();
	}
}

bool Labonatip_GUI::loadProtocol()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::loadProtocol :::  "
		<< m_protocol_path.toStdString() << "  " << endl;

	QString file_name = QFileDialog::getOpenFileName(
		this, m_str_save_protocol, m_protocol_path,  // dialog to open files
		"Lab-on-a-tip protocol File (*.prt);; All Files(*.*)", 0);

	if (file_name.isEmpty()) {
		//QMessageBox::warning(this, m_str_warning,
		//	m_str_file_not_found + "<br>" + file_name);
		return false;
	}
	
	
	if (m_reader->readProtocol(ui->treeWidget_macroTable, file_name))
	{
		addAllCommandsToProtocol();
		m_current_protocol_file_name = file_name;
		return true;
	}
	return false;
}

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

void Labonatip_GUI::savePressed()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::savePressed   " << endl;

	if (ui->stackedWidget_main->currentIndex() == 0)
	{
		saveSettingsFile();
	}
	else
	{
		saveProtocol();
	}

}

bool Labonatip_GUI::saveProtocol()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::saveProtocol    " << endl;

    // if we are working with a new protocol, saveAs	
	if (m_current_protocol_file_name.isEmpty())
	{
		if (!saveProtocolAs()) return false;
		else return true;
	}

	// if the name is not empty, we ask if the user want to save as
	QMessageBox::StandardButton resBtn = QMessageBox::question(this, m_str_warning,
		m_str_current_prot_name + "<br>" + m_current_protocol_file_name +
		"<br>" + m_str_question_override + "<br>" + m_str_override_guide,
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes) {
		// yes = override
		if (!m_writer->saveProtocol(ui->treeWidget_macroTable, m_current_protocol_file_name)) {
			QApplication::restoreOverrideCursor();    
			QMessageBox::warning(this, m_str_warning, 
				m_str_file_not_saved + "<br>" + m_current_protocol_file_name);
			return false;
		}
	}
	if (resBtn == QMessageBox::No)
	{ //no = save as
		if (!saveProtocolAs()) return false;
	}
	if (resBtn == QMessageBox::Cancel)
	{
		//do nothing
	}

	return true;
}

bool Labonatip_GUI::saveProtocolAs()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::saveProtocolAs    " << endl;

	QString file_name = QFileDialog::getSaveFileName(this,
		m_str_save_protocol, m_protocol_path,  // dialog to open files
		"Lab-on-a-tip protocol File (*.prt);; All Files(*.*)", 0);
	

	if (!m_writer->saveProtocol(ui->treeWidget_macroTable, file_name)) {
		QApplication::restoreOverrideCursor();    
		QMessageBox::warning(this, m_str_warning, 
			m_str_file_not_saved + "<br>" + file_name);
		return false;
	}
	readProtocolFolder(m_protocol_path);
	m_current_protocol_file_name = file_name; //TODO: this does not work properly
	QApplication::restoreOverrideCursor();    
	return true;
}

void Labonatip_GUI::saveSettingsFile() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::saveFile    " << endl;

	QString _path = QFileDialog::getSaveFileName(this, 
		m_str_save_profile, m_settings_path,  // dialog to open files
		"Profile file (*.ini);; All Files(*.*)", 0);

	if (_path.isEmpty()) { 
	    QMessageBox::information(this, m_str_information,
			m_str_cannot_save_profile + "<br>" + _path);
		return;
	}

	if (!m_dialog_tools->setFileNameAndSaveSettings(_path)) {
		QMessageBox::warning(this, m_str_warning, 
			m_str_cannot_save_profile + "<br>" + _path);
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
	m_dialog_tools->setExtDataPath(m_ext_data_path); 
}


void Labonatip_GUI::showProtocolEditor() { 

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showProtocolEditor    " << endl;

	// if we are in the commander, we visualize the editor
	if (ui->stackedWidget_main->currentIndex() == 0) {
		ui->stackedWidget_main->setCurrentIndex(1);
		ui->stackedWidget_indock->setCurrentIndex(1);
		ui->actionSave_profile->setText(m_str_save);
		ui->actionLoad_profile->setText(m_str_load);
		ui->actionEditor->setText(m_str_commander);
		ui->actionEditor->setIcon(QIcon(":/icons/controls.png")); 
	}
	//otherwise we are in the editor and we need to get back to the commander
	else
	{
		ui->stackedWidget_main->setCurrentIndex(0);
		ui->stackedWidget_indock->setCurrentIndex(0);
		ui->actionSave_profile->setText(m_str_save_profile);
		ui->actionLoad_profile->setText(m_str_load_profile);
		ui->actionEditor->setText(m_str_editor);
		QIcon iconEditor;
		iconEditor.addFile(QStringLiteral("://icons/protocolEditor.png"), QSize(), QIcon::Normal, QIcon::Off);
		iconEditor.addFile(QStringLiteral(":/icons/protocolEditor_off.png"), QSize(), QIcon::Disabled, QIcon::Off);
		ui->actionEditor->setIcon(iconEditor);

		//update the chart
		m_labonatip_chart_view->updateChartProtocol(m_protocol);

		// visualize duration in the chart information panel 
		m_protocol_duration = m_ppc1->protocolDuration(*m_protocol);
		QString s = QString::number(m_protocol_duration);
		s.append(" s");
		ui->label_duration->setText(s);

	}
	return;
}


void Labonatip_GUI::simulationOnly()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::simulationOnly    " 
		<< ui->actionSimulation->isChecked() << endl;

	this->stopSolutionFlow();

	m_simulationOnly = ui->actionSimulation->isChecked();

	ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
	m_macroRunner_thread->setSimulationFlag(m_simulationOnly);

	ui->groupBox_action->setEnabled(m_simulationOnly || ui->actionConnectDisconnect->isChecked());
	ui->groupBox_deliveryZone->setEnabled(m_simulationOnly || ui->actionConnectDisconnect->isChecked());
	ui->groupBox_3->setEnabled(m_simulationOnly || ui->actionConnectDisconnect->isChecked());
	//ui->tab_2->setEnabled(m_simulationOnly || ui->actionConnectDisconnect->isChecked());
	enableTab2(m_simulationOnly || ui->actionConnectDisconnect->isChecked());

}

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
		QApplication::setOverrideCursor(Qt::WaitCursor);   

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
	QApplication::restoreOverrideCursor();   

}


bool Labonatip_GUI::disCon(bool _connect)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::disCon    " << endl;

	

	if (m_simulationOnly) {
		QMessageBox::information(this, 
			m_str_warning, m_str_warning_simulation_only);
		return false;
	}

	try
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);   

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
						QString(m_str_cannot_connect_ppc1 + "\n" 
							+ m_str_cannot_connect_ppc1_check_cables));

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
								QString(m_str_cannot_connect_ppc1_twice + "\n" + 
									m_str_cannot_connect_ppc1_check_cables));
							m_pipette_active = false;
							ui->actionConnectDisconnect->setChecked(false);
							return false;  // we could not connect twice
						}
					}
				}

				// if we are here the PPC1 is connected 
				QThread::msleep(250);  // just to be sure that the device is properly running

				// run the ppc1 thread
				m_ppc1->run();   // TODO: this is not the best way of running the device as it cannot handle exceptions
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
					ui->groupBox_action->setEnabled(true);
					ui->groupBox_deliveryZone->setEnabled(true);
					ui->groupBox_3->setEnabled(true);
					//ui->tab_2->setEnabled(true);
					enableTab2(true);
					ui->actionReboot->setEnabled(true);
					ui->actionShudown->setEnabled(true);
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
					QApplication::restoreOverrideCursor();    
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
				ui->actionConnectDisconnect->setChecked(true);
				return false; // it should be false, but in this case is a user choice? 
			}

			if (!m_ppc1->isRunning()) return true; // already stop

			// stop the PPC1
			QApplication::setOverrideCursor(Qt::WaitCursor);    
			this->stopSolutionFlow(); 
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
				ui->groupBox_action->setEnabled(false);
				ui->groupBox_deliveryZone->setEnabled(false);
				ui->groupBox_3->setEnabled(false);
				//ui->tab_2->setEnabled(false);
				enableTab2(false);
				ui->actionReboot->setEnabled(false);
				ui->actionShudown->setEnabled(false);
				QApplication::restoreOverrideCursor();    
				return true;
			}
			else {  // the device was not stopped
				ui->actionConnectDisconnect->setChecked(false);
				m_update_GUI->stop();
				this->setStatusLed(true);
				ui->status_PPC1_label->setText(m_str_PPC1_status_con);
				ui->actionConnectDisconnect->setText(m_str_disconnect);
				ui->actionSimulation->setEnabled(false);
				ui->actionSimulation->setEnabled(true);
				ui->groupBox_action->setEnabled(true);
				ui->groupBox_3->setEnabled(true);
				//ui->tab_2->setEnabled(true);
				enableTab2(true);
				ui->actionReboot->setEnabled(true);
				ui->actionShudown->setEnabled(true);
				QMessageBox::information(this, m_str_warning,
					m_str_unable_stop_ppc1);
				QApplication::restoreOverrideCursor();   
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

	QApplication::setOverrideCursor(Qt::WaitCursor);
	setEnableMainWindow(false);

	if (m_pipette_active) {
		// with the pipette active this will stop the threads
		disCon(false); 

		// if it is still active, the disconnection failed and and we cannot continue
		if (m_pipette_active) { 
			setEnableMainWindow(true);
			QApplication::restoreOverrideCursor();  
			return;
		}

		// activate connection mode
		ui->actionSimulation->setChecked(false);
	
		//give the command to reboot
		m_ppc1->reboot();

		// reset to 0
		updatePonSetPoint(0.0);
		updatePoffSetPoint(0.0);
		updateVrecircSetPoint(0.0);
		updateVswitchSetPoint(0.0);

		// wait 20 seconds
		if (!visualizeProgressMessage(20, m_str_rebooting)) return; 

		// try to reconnect
		disCon(true); 
		if (!visualizeProgressMessage(5, m_str_reconnecting)) return;
	}

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();  
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
			ui->toolBar_2->iconSize().width() + 16; //12px is to avoid the widget to go to a new line

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
	// it does nothing if the advanced tab is closed
	if (!ui->actionAdvanced->isChecked()) return;  

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