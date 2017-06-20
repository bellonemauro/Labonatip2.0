/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"



void Labonatip_GUI::operationalMode() {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::operationalMode    " << endl;

	if (m_pipette_active) {
		if (m_ppc1->isConnected()) m_ppc1->closeAllValves();
	}

	// if the sliders were in the same position, it does not change, so it does not send the command !!! 
	// so the event slider_changed is not emitted
	ui->horizontalSlider_recirculation->setValue(115);
	ui->horizontalSlider_switch->setValue(115);  // on slider changed, it send the value to the pumps
	QThread::sleep(5);
	ui->horizontalSlider_p_off->setValue(21);
	ui->horizontalSlider_p_on->setValue(190);


	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::runMacro() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::runMacro    " << endl;

	if (!m_macroRunner_thread->isRunning()) {

		m_macroRunner_thread->setMacroPrt(m_macro);

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		m_macroRunner_thread->start();

		ui->groupBox_deliveryZone->setEnabled(false);
		ui->pushButton_operational->setEnabled(false);
		ui->pushButton_newTip->setEnabled(false);
		ui->pushButton_standby->setEnabled(false);
		ui->pushButton_stop->setEnabled(false);
		ui->tabWidget->setEnabled(false);
		setEnableSolutionButtons(false);
		ui->actionDisCon->setEnabled(false);
		ui->actionRun->setEnabled(false);
		ui->actionReset->setEnabled(false);
		ui->actionSleep->setEnabled(false);
		ui->actionShutdown->setEnabled(false);
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
		ui->pushButton_stop->setEnabled(true);
		ui->tabWidget->setEnabled(true);
		setEnableSolutionButtons(true);
		ui->actionDisCon->setEnabled(!m_simulationOnly);
		ui->actionRun->setEnabled(!m_simulationOnly);
		ui->actionReset->setEnabled(!m_simulationOnly);
		ui->actionSleep->setEnabled(!m_simulationOnly);
		ui->actionShutdown->setEnabled(!m_simulationOnly);
		ui->label_runMacro->setText("Run Macro");
	}

}


void Labonatip_GUI::newTip()
{


	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::newTip    " << endl;

	//Ask: Place the pipette into the holder and tighten.THEN PRESS OK.
	QMessageBox::information(this, " Information ",
		"Place the pipette into the holder and tighten.THEN PRESS OK");

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

														//vf0
	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}

	//D0
	ui->horizontalSlider_p_on->setValue(0);

	//C0
	ui->horizontalSlider_p_off->setValue(0);

	//B0
	ui->horizontalSlider_switch->setValue(0);

	//A0
	ui->horizontalSlider_recirculation->setValue(0);

	//Wait 5 seconds
	int counter = 5;
	QProgressDialog *PD = new QProgressDialog(" waiting ...", "Cancel", 0, counter, this);
	PD->setMinimumWidth(350);
	PD->setMinimumHeight(150);
	PD->show();
	PD->setValue(counter);
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < counter; i++) {
		PD->setValue(i);
		QThread::sleep(1);  //--> wait the last execution
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Running new tip cancelled  ");
			return;
		}
	}
	PD->cancel();
	//QThread::sleep(5);

	//D200
	ui->horizontalSlider_p_on->setValue(200);

	//Wait 5 seconds
	//QThread::sleep(5);
	PD->show();
	counter = 5;
	PD->setValue(counter);
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < counter; i++) {
		PD->setValue(i);
		QThread::sleep(1);  //--> wait the last execution
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Running new tip cancelled  ");
			return;
		}
	}
	PD->cancel();

	//vff
	if (m_pipette_active) {
		m_ppc1->openAllValves();
	}

	//Ask : wait until a droplet appears at the tip of the pipette and THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, " Information ",
		"wait until a droplet appears at the tip of the pipette and THEN PRESS OK");
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

														//Wait 40 seconds
														//QThread::sleep(40);
	PD->show();
	counter = 40;
	PD->setMaximum(counter);
	PD->setValue(counter);
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < counter; i++) {
		PD->setValue(i);
		QThread::sleep(1);  //--> wait the last execution
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Running new tip cancelled  ");
			return;
		}
	}
	PD->cancel();

	//vf0
	if (m_pipette_active) {
		m_ppc1->closeAllValves();
	}

	//D0
	ui->horizontalSlider_p_on->setValue(0);

	//Wait 10 seconds
	//QThread::sleep(10);
	PD->show();
	counter = 10;
	PD->setMaximum(counter);
	PD->setValue(counter);
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < counter; i++) {
		PD->setValue(i);
		QThread::sleep(1);  //--> wait the last execution
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Running new tip cancelled  ");
			return;
		}
	}
	PD->cancel();

	//Ask : Remove the droplet using a lens tissue and put the pipette into solution.THEN PRESS OK.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, " Information ",
		"Remove the droplet using a lens tissue and put the pipette into solution.THEN PRESS OK");
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

														//B - 200
	ui->horizontalSlider_switch->setValue(200);

	//A - 200
	ui->horizontalSlider_recirculation->setValue(200);

	//Wait 90 seconds
	//QThread::sleep(90);
	PD->show();
	counter = 90;
	PD->setMaximum(counter);
	PD->setValue(counter);
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < counter; i++) {
		PD->setValue(i);
		QThread::sleep(1);  //--> wait the last execution
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Running new tip cancelled  ");
			return;
		}
	}
	PD->cancel();

	//C21
	ui->horizontalSlider_p_off->setValue(21);

	//D190
	ui->horizontalSlider_p_on->setValue(190);

	//Wait 5 seconds
	QThread::sleep(5);
	PD->show();
	counter = 5;
	PD->setMaximum(counter);
	PD->setValue(counter);
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < counter; i++) {
		PD->setValue(i);
		QThread::sleep(1);  //--> wait the last execution
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Running new tip cancelled  ");
			return;
		}
	}
	PD->cancel();

	//B - 115
	ui->horizontalSlider_switch->setValue(115);

	//A - 115
	ui->horizontalSlider_recirculation->setValue(115);

	//Ask: Pipette is ready for operation.PRESS OK TO START.
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	QMessageBox::information(this, " Information ",
		"Pipette is ready for operation.PRESS OK TO START");



}

void Labonatip_GUI::macroFinished(const QString &_result) {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::macroFinished    " << endl;

	QMessageBox::information(this, " Information !", _result);
	ui->statusBar->showMessage("MACRO FINISHED");
	ui->label_runMacro->setText("Run Macro");
	ui->groupBox_deliveryZone->setEnabled(true);
	ui->pushButton_operational->setEnabled(true);
	ui->pushButton_newTip->setEnabled(true);
	ui->pushButton_stop->setEnabled(true);
	ui->pushButton_standby->setEnabled(true);
	ui->tabWidget->setEnabled(true);
	setEnableSolutionButtons(true);
	ui->actionDisCon->setEnabled(!m_simulationOnly);
	ui->actionRun->setEnabled(!m_simulationOnly);
	ui->actionReset->setEnabled(!m_simulationOnly);
	ui->actionSleep->setEnabled(!m_simulationOnly);
	ui->actionShutdown->setEnabled(!m_simulationOnly);

}


void Labonatip_GUI::shutdown() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
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
		ui->horizontalSlider_p_on->setValue(0);
		ui->horizontalSlider_p_off->setValue(0);
		QThread::sleep(10);
		ui->horizontalSlider_recirculation->setValue(0);
		ui->horizontalSlider_switch->setValue(0);
		QThread::sleep(15);
		if (m_pipette_active) {
			m_ppc1->pumpingOff();
		}

	}
	else {
		// do nothing for now
	}
	setEnableMainWindow(true);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::standby()
{
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::standby   " << endl;

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
	ui->horizontalSlider_p_on->setValue(0);
	ui->horizontalSlider_p_off->setValue(11);

	QThread::sleep(5);
	ui->horizontalSlider_switch->setValue(45);
	ui->horizontalSlider_recirculation->setValue(45);

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}
