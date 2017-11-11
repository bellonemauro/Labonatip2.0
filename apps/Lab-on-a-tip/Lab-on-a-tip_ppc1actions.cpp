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
	QMessageBox::information(this, " Information ",
		"Place the pipette into the holder and tighten.THEN PRESS OK");
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

														//vf0
	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}

	//D0
	updatePonSetPoint(0.0);

	//C0
	updatePoffSetPoint(0.0);

	//B0
	updateVswitchSetPoint(0.0);

	//A0
	updateVrecircSetPoint(0.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, "Initialization.")) return;

	//D200
	updatePonSetPoint(200.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, "Pressurize the system.")) return;

	//vff
	if (m_pipette_active) {
		m_ppc1->openAllValves();
	}

	//Ask : wait until a droplet appears at the tip of the pipette and THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, " Information ",
		"Wait until a droplet appears at the tip of the pipette and THEN PRESS OK");
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//Wait 40 seconds
	if (!visualizeProgressMessage(40, "Purging the liquid channels.")) return;

	//vf0
	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}

	//D0
	updatePonSetPoint(0.0);

	//Wait 10 seconds
	if (!visualizeProgressMessage(10, "Still purging the liquid channels.")) return;

	//Ask : Remove the droplet using a lens tissue and put the pipette into solution.THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, " Information ",
		"Remove the droplet using a lens tissue. THEN PRESS OK");
	QMessageBox::information(this, " Information ",
		"Put the pipette into solution. THEN PRESS OK");
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	//B - 200
	updateVswitchSetPoint(200.0);

	//A - 200
	updateVrecircSetPoint(200.0);

	//Wait 90 seconds
	if (!visualizeProgressMessage(90, "Purging the vacuum  channels.")) return;

	//C21
	updatePoffSetPoint(default_poff);// (21.0);

	//D190
	updatePonSetPoint(default_pon);// (190.0);

	//Wait 5 seconds
	if (!visualizeProgressMessage(5, "Establishing operational pressures.")) return;

	//B - 115
	updateVswitchSetPoint(default_v_switch);// (115);

	//A - 115
	updateVrecircSetPoint(default_v_recirc);// (115);

	//Ask: Pipette is ready for operation.PRESS OK TO START.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, " Information ",
		"Pipette is ready for operation. PRESS OK TO START");

	setEnableMainWindow(true);

}


void Labonatip_GUI::runMacro() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::runMacro    " << endl;

	if (!m_macroRunner_thread->isRunning()) {

		if (!m_macro) {
			QMessageBox::information(this, "Lab-on-a-tip information ",
				"No macro loaded, load a macro first");
				return;
		}
		else {
			QString macro_path = m_dialog_tools->getMacroPath();
			QString msg = tr("The macro loaded is : \n");
			msg.append(macro_path);
			msg.append("\n press ''ok'' to run the macro, or press ''cancel'' to load a new macro. ");
			QMessageBox::StandardButton resBtn = 
				QMessageBox::question(this, "Lab-on-a-tip information ", msg,
				QMessageBox::Cancel | QMessageBox::Ok,
				QMessageBox::Ok);
			if (resBtn != QMessageBox::Cancel) {
				// do nothing for now

			}
			else {
				// do nothing for now
				m_macroRunner_thread->disconnect();

				m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
				m_macroRunner_thread->killMacro(true);


				ui->groupBox_deliveryZone->setEnabled(true);
				ui->pushButton_operational->setEnabled(true);
				ui->pushButton_newTip->setEnabled(true);
				ui->pushButton_standby->setEnabled(true);
				ui->pushButton_stop->setEnabled(true);
				//ui->tabWidget->setEnabled(false);
				ui->tab_2->setEnabled(true);
				ui->tab_4->setEnabled(true);
				setEnableSolutionButtons(true);
				ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
				ui->actionSimulation->setEnabled(!m_simulationOnly);
				ui->actionReboot->setEnabled(!m_simulationOnly);
				ui->actionShudown->setEnabled(!m_simulationOnly);
				ui->label_runMacro->setText("Run Macro");
				return;
			}
		}

		m_macroRunner_thread->setMacroPrt(m_macro);
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_GUI::runMacro    RUNNING" << endl;
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
		//ui->tabWidget->setEnabled(false);
		ui->tab_2->setEnabled(false);
		ui->tab_4->setEnabled(false);
		setEnableSolutionButtons(false);
		ui->actionConnectDisconnect->setEnabled(false);
		ui->actionSimulation->setEnabled(false);
		ui->actionReboot->setEnabled(false);
		ui->actionShudown->setEnabled(false);
		ui->label_runMacro->setText("Stop Macro");
	}
	else {
		m_macroRunner_thread->disconnect();

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		m_macroRunner_thread->killMacro(true);


		ui->groupBox_deliveryZone->setEnabled(true);
		ui->pushButton_operational->setEnabled(true);
		ui->pushButton_newTip->setEnabled(true);
		ui->pushButton_standby->setEnabled(true);
		//ui->pushButton_stop->setEnabled(true);
		//ui->tabWidget->setEnabled(false);
		ui->tab_2->setEnabled(true);
		ui->tab_4->setEnabled(true);
		setEnableSolutionButtons(true);
		ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
		ui->actionSimulation->setEnabled(m_simulationOnly);
		ui->actionReboot->setEnabled(!m_simulationOnly);
		ui->actionShudown->setEnabled(!m_simulationOnly);
		ui->label_runMacro->setText("Run Macro");
	}

}


void Labonatip_GUI::macroFinished(const QString &_result) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::macroFinished    " << endl;

	QMessageBox::information(this, " Information !", _result);
	//ui->statusBar->showMessage("MACRO FINISHED");
	ui->label_runMacro->setText("Run Macro");
	ui->groupBox_deliveryZone->setEnabled(true);
	ui->pushButton_operational->setEnabled(true);
	ui->pushButton_newTip->setEnabled(true);
	ui->pushButton_stop->setEnabled(true);
	ui->pushButton_standby->setEnabled(true);
	//ui->tabWidget->setEnabled(false);
	ui->tab_2->setEnabled(true);
	ui->tab_4->setEnabled(true);
	setEnableSolutionButtons(true);
	ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
	ui->actionSimulation->setEnabled(m_simulationOnly);
	ui->actionReboot->setEnabled(!m_simulationOnly);
	ui->actionShudown->setEnabled(!m_simulationOnly);

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

	if (m_pipette_active) {
		if (m_ppc1->isConnected()) m_ppc1->closeAllValves();
	}

	updateVrecircSetPoint(default_v_recirc);// update the set point
	updateVswitchSetPoint(default_v_switch);// update the set point
	if (!visualizeProgressMessage(5, " waiting ... ")) return;
	updatePoffSetPoint(default_poff);// update the set point
	updatePonSetPoint(default_pon);// update the set point

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}


void Labonatip_GUI::stopSolutionFlow()
{
	// look for the active flow
	if (ui->pushButton_solution1->isChecked()) {
		ui->pushButton_solution1->setChecked(false);
		pushSolution1(); // if the flow is active, this should stop it!

		updateDrawing(-1);
		ui->pushButton_stop->released();
		ui->pushButton_stop->setEnabled(false);
		return;
	}

	if (ui->pushButton_solution2->isChecked()) {
		ui->pushButton_solution2->setChecked(false);
		pushSolution2();
		updateDrawing(-1);
		ui->pushButton_stop->released();
		ui->pushButton_stop->setEnabled(false);
		return;
	}

	if (ui->pushButton_solution3->isChecked()) {
		ui->pushButton_solution3->setChecked(false);
		pushSolution3();
		updateDrawing(-1);
		ui->pushButton_stop->released();
		ui->pushButton_stop->setEnabled(false);
		return;
	}

	if (ui->pushButton_solution4->isChecked()) {
		ui->pushButton_solution4->setChecked(false);
		pushSolution4();
		updateDrawing(-1);
		ui->pushButton_stop->released();
		ui->pushButton_stop->setEnabled(false);
		return;
	}
	// if none is checked, do nothing.




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

	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}
	updatePonSetPoint(0.0);
	updatePoffSetPoint(11.0);

	if (!visualizeProgressMessage(5, " Standby operation progress ")) return;
	updateVswitchSetPoint(45.0);
	updateVrecircSetPoint(45.0);

	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

