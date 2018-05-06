/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"

void Labonatip_GUI::newTip()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::newTip    " << endl;

	setEnableMainWindow(false);

	//Ask: Place the pipette into the holder and tighten.THEN PRESS OK.
	QMessageBox::information(this, m_str_information,
		m_str_newtip_msg1);
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	
	//vf0
	closeAllValves();

	//D0
	updatePonSetPoint(0.0);

	//C0
	updatePoffSetPoint(0.0);

	//B0
	updateVswitchSetPoint(0.0);

	//A0
	updateVrecircSetPoint(0.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, m_str_initialization)) return;

	//D200
	updatePonSetPoint(200.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, m_str_newtip_msg2)) return;

	//vff
	if (m_pipette_active) {
		m_ppc1->openAllValves();
	}

	//Ask : wait until a droplet appears at the tip of the pipette and THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, m_str_information,
		m_str_newtip_msg3);
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//Wait 40 seconds
	if (!visualizeProgressMessage(40, m_str_newtip_msg4)) return;

	//vf0
	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}

	//D0
	updatePonSetPoint(0.0);

	//Wait 10 seconds
	if (!visualizeProgressMessage(10, m_str_newtip_msg5)) return;

	//Ask : Remove the droplet using a lens tissue and put the pipette into solution.THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, m_str_information,
		m_str_newtip_msg6);
	QMessageBox::information(this, m_str_information,
		m_str_newtip_msg7);
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//B - 200
	updateVswitchSetPoint(200.0);

	//A - 200
	updateVrecircSetPoint(200.0);

	//Wait 90 seconds
	if (!visualizeProgressMessage(90, m_str_newtip_msg8)) return;

	//C21
	updatePoffSetPoint(m_pr_params->p_off_default );// (21.0);

	//D190
	updatePonSetPoint(m_pr_params->p_on_default );// (190.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, m_str_newtip_msg9)) return;

	//B - 115
	updateVswitchSetPoint(-m_pr_params->v_switch_default);// (115);

	//A - 115
	updateVrecircSetPoint(-m_pr_params->v_recirc_default );// (115);

	//Ask: Pipette is ready for operation.PRESS OK TO START.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, m_str_information,
		m_str_newtip_msg10);

	setEnableMainWindow(true);

}


void Labonatip_GUI::runMacro() { 

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::runMacro    " << endl;

	if (!m_macroRunner_thread->isRunning()) {

		if (!m_protocol) {
			QMessageBox::information(this, m_str_information,
				m_str_no_protocol_load_first);
				return;
		}

		
		QString macro_path = m_dialog_p_editor->getProtocolPath();
		QString msg = m_str_loaded_protocol_is;
		msg.append(macro_path);
		msg.append(m_str_protocol_confirm);
		QMessageBox::StandardButton resBtn = 
			QMessageBox::question(this, m_str_information, msg,
			QMessageBox::Cancel | QMessageBox::Ok,
			QMessageBox::Ok);
		if (resBtn == QMessageBox::Cancel) 
		{
			m_macroRunner_thread->disconnect();

			m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
			m_macroRunner_thread->killMacro(true);

			m_ppc1->setVerbose(m_pr_params->verboseOut);
			ui->groupBox_deliveryZone->setEnabled(true);
			ui->pushButton_operational->setEnabled(true);
			ui->pushButton_newTip->setEnabled(true);
			ui->pushButton_standby->setEnabled(true);
			ui->pushButton_stop->setEnabled(true);
			ui->toolBar_2->setEnabled(true);
			//ui->tabWidget->setEnabled(false);
			ui->tab_2->setEnabled(true);
			ui->tab_4->setEnabled(true);
			setEnableSolutionButtons(true);
			ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
			ui->actionSimulation->setEnabled(!m_simulationOnly);
			ui->actionReboot->setEnabled(!m_simulationOnly);
			ui->actionShudown->setEnabled(!m_simulationOnly);
			ui->label_runMacro->setText(m_str_label_run_protocol);

			QString s = " Protocol execution stopped : ";
			s.append(m_dialog_p_editor->getProtocolName());
			int remaining_time_sec = m_protocol_duration - 0 * m_protocol_duration / 100;
			s.append(" ----- remaining time,  ");
			int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
			int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
			int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
			s.append(QString::number(remaining_hours));
			s.append(" h,   ");
			s.append(QString::number(remaining_mins));
			s.append(" min,   ");
			s.append(QString::number(remaining_secs));
			s.append(" sec   ");
			ui->progressBar_macroStatus->setValue(0);
			ui->label_macroStatus->setText(s);

			return;
		}

		// if ok was pressed
		m_ppc1->setVerbose(false);
		m_macroRunner_thread->setProtocol(m_protocol);
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_GUI::runMacro " <<   msg.toStdString() << endl;

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		connect(m_macroRunner_thread,
			&Labonatip_macroRunner::resultReady, this,
			&Labonatip_GUI::macroFinished);

		connect(m_macroRunner_thread,
			&Labonatip_macroRunner::sendStatusMessage, this,
			&Labonatip_GUI::updateMacroStatusMessage);

		connect(m_macroRunner_thread,
			&Labonatip_macroRunner::timeStatus, this,
			&Labonatip_GUI::updateMacroTimeStatus);

		connect(m_macroRunner_thread,
			&Labonatip_macroRunner::sendAskMessage, this,
			&Labonatip_GUI::askMessage);

		m_macroRunner_thread->start();

		ui->groupBox_deliveryZone->setEnabled(false);
		ui->pushButton_operational->setEnabled(false);
		ui->pushButton_newTip->setEnabled(false);
		ui->pushButton_standby->setEnabled(false);
		ui->pushButton_stop->setEnabled(false);
		ui->toolBar_2->setEnabled(false);
		//ui->tabWidget->setEnabled(false);
		ui->tab_2->setEnabled(false);
		ui->tab_4->setEnabled(false);
		setEnableSolutionButtons(false);

		if (!ui->actionConnectDisconnect->isChecked()) { 
			ui->actionSimulation->setEnabled(true); 
		}
		else { 
			ui->actionSimulation->setEnabled(false); 
		}
		//ui->actionSimulation->setEnabled(false);
		ui->actionReboot->setEnabled(false);
		ui->actionShudown->setEnabled(false);
		ui->label_runMacro->setText(m_str_label_stop_protocol);
	}
	else {
		m_macroRunner_thread->disconnect();

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		m_macroRunner_thread->killMacro(true);

		m_ppc1->setVerbose(m_pr_params->verboseOut);
		ui->groupBox_deliveryZone->setEnabled(true);
		ui->pushButton_operational->setEnabled(true);
		ui->pushButton_newTip->setEnabled(true);
		ui->pushButton_standby->setEnabled(true);
		ui->toolBar_2->setEnabled(true);
		ui->pushButton_stop->setEnabled(true);
		//ui->tabWidget->setEnabled(false);
		ui->tab_2->setEnabled(true);
		ui->tab_4->setEnabled(true);
		setEnableSolutionButtons(true);
		ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
		if (!ui->actionConnectDisconnect->isChecked()) { 
			ui->actionSimulation->setEnabled(true); 
		}
		else { 
			ui->actionSimulation->setEnabled(false); 
		}
			//ui->actionSimulation->setEnabled(m_simulationOnly);

		ui->actionReboot->setEnabled(!m_simulationOnly);
		ui->actionShudown->setEnabled(!m_simulationOnly);
		ui->label_runMacro->setText(m_str_label_run_protocol);
	}

}


void Labonatip_GUI::macroFinished(const QString &_result) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::macroFinished    " << endl;

	QMessageBox::information(this, m_str_information, _result);
	ui->label_runMacro->setText(m_str_label_run_protocol);
	ui->groupBox_deliveryZone->setEnabled(true);
	ui->pushButton_operational->setEnabled(true);
	ui->pushButton_newTip->setEnabled(true);
	ui->pushButton_stop->setEnabled(true);
	ui->pushButton_standby->setEnabled(true);
	ui->toolBar_2->setEnabled(true);
	//ui->tabWidget->setEnabled(false);
	ui->tab_2->setEnabled(true);
	ui->tab_4->setEnabled(true);
	setEnableSolutionButtons(true);
	ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
	if (!ui->actionConnectDisconnect->isChecked()) { 
		ui->actionSimulation->setEnabled(true);
	}
	else { 
		ui->actionSimulation->setEnabled(false); 
	}

	ui->actionReboot->setEnabled(!m_simulationOnly);
	ui->actionShudown->setEnabled(!m_simulationOnly);

	updateVrecircSetPoint(-m_ppc1->m_PPC1_data->channel_A->set_point);
	updateVswitchSetPoint(-m_ppc1->m_PPC1_data->channel_B->set_point);
	updatePoffSetPoint(m_ppc1->m_PPC1_data->channel_C->set_point);
	updatePonSetPoint(m_ppc1->m_PPC1_data->channel_D->set_point);

	disconnect(m_macroRunner_thread,
		&Labonatip_macroRunner::resultReady, this,
		&Labonatip_GUI::macroFinished);

	disconnect(m_macroRunner_thread,
		&Labonatip_macroRunner::sendStatusMessage, this,
		&Labonatip_GUI::updateMacroStatusMessage);

	disconnect(m_macroRunner_thread,
		&Labonatip_macroRunner::timeStatus, this,
		&Labonatip_GUI::updateMacroTimeStatus);

	disconnect(m_macroRunner_thread,
		&Labonatip_macroRunner::sendAskMessage, this,
		&Labonatip_GUI::askMessage);

}


void Labonatip_GUI::operationalMode() {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	setEnableMainWindow(false);

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::operationalMode    " << endl;

	//vf0
	closeAllValves();
	

	updateVrecircSetPoint(-m_pr_params->v_recirc_default );// update the set point
	updateVswitchSetPoint(-m_pr_params->v_switch_default );// update the set point
	if (!visualizeProgressMessage(5, m_str_waiting)) return;
	updatePoffSetPoint(m_pr_params->p_off_default );// update the set point
	updatePonSetPoint(m_pr_params->p_on_default);// update the set point

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}


void Labonatip_GUI::stopFlow()
{
	//  AllOff()
	//	setPoff(0)
	//	setPon(0)
	//	sleep(3)
	//	setVswitch(0)
	//	setVrecirc(0)
	//	sleep(3)
	closeAllValves();
	updatePoffSetPoint(0.0);
	updatePonSetPoint(0.0);
	if (!visualizeProgressMessage(3, m_str_stop_1)) return;
	updateVswitchSetPoint(0.0);
	updateVrecircSetPoint(0.0);
	if (!visualizeProgressMessage(3, m_str_stop_2)) return;

	return;
}

void Labonatip_GUI::stopSolutionFlow()
	{
	// look for the active flow
	if (ui->pushButton_solution1->isChecked()) {
		ui->pushButton_solution1->setChecked(false);
		pushSolution1(); // if the flow is active, this should stop it!

		updateDrawing(-1);
		//ui->pushButton_stop->released();
		//ui->pushButton_stop->setEnabled(false);
		return;
	}

	if (ui->pushButton_solution2->isChecked()) {
		ui->pushButton_solution2->setChecked(false);
		pushSolution2();
		updateDrawing(-1);
		//ui->pushButton_stop->released();
		//ui->pushButton_stop->setEnabled(false);
		return;
	}

	if (ui->pushButton_solution3->isChecked()) {
		ui->pushButton_solution3->setChecked(false);
		pushSolution3();
		updateDrawing(-1);
		//ui->pushButton_stop->released();
		//ui->pushButton_stop->setEnabled(false);
		return;
	}

	if (ui->pushButton_solution4->isChecked()) {
		ui->pushButton_solution4->setChecked(false);
		pushSolution4();
		updateDrawing(-1);
		//ui->pushButton_stop->released();
		//ui->pushButton_stop->setEnabled(false);
		return;
	}

	return;
}


void Labonatip_GUI::standby()
{
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::standby   " << endl;

	setEnableMainWindow(false);

	//OLD SLEEP MACRO
	// allOff()
	// setPoff(11)
	// setPon(0)
	// sleep(5)
	// setVswitch(-45)
	// setVrecirc(-45)
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//vf0
	closeAllValves();
	

	updatePonSetPoint(45.0);
	updatePoffSetPoint(11.0);

	if (!visualizeProgressMessage(5, m_str_standby_operation)) return;
	updateVswitchSetPoint(45.0);
	updateVrecircSetPoint(45.0);

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

