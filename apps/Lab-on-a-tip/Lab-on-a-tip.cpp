/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip.h"
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAbstractAxis>

Labonatip_GUI::Labonatip_GUI(QMainWindow *parent) :
	QMainWindow(parent),
	ui(new Ui::Labonatip_GUI),
	m_pipette_active(false),
	m_ppc1 ( new fluicell::PPC1api() ),
	c_x(53.0),
	c_y(46.0),
	c_radius(10.0),
	g_radius(70.0),
	m_pen_line_width(7),
	l_x1(-24.0),
	l_y1(49.0),
	l_x2(55.0),
	l_y2(l_y1),
	m_base_time_step(1000),
	m_sol1_color(QColor::fromRgb(189, 62, 71)),
	m_sol1_color_g05(QColor::fromRgb(200, 100, 110, 255)),
	m_sol1_color_g1(QColor::fromRgb(220, 150, 150, 255)),
	m_sol2_color(QColor::fromRgb(96, 115, 158)),
	m_sol2_color_g05(QColor::fromRgb(130, 155, 195, 255)),
	m_sol2_color_g1(QColor::fromRgb(156, 195, 229, 255)),
	m_sol3_color(QColor::fromRgb(193, 130, 50)),
	m_sol3_color_g05(QColor::fromRgb(255, 180, 85, 255)),
	m_sol3_color_g1(QColor::fromRgb(255, 225, 120, 255)),
	m_sol4_color(QColor::fromRgb(83, 155, 81)),
	m_sol4_color_g05(QColor::fromRgb(120, 240, 120, 255)),
	m_sol4_color_g1(QColor::fromRgb(150, 240, 150, 255))
{
  // allows to use path alias
  QDir::setSearchPaths("icons", QStringList(QDir::currentPath() + "/icons/"));

  // setup the user interface
  ui->setupUi (this);
  ui->dockWidget->close();  //close the advaced dock page

  // set the stylesheet for the ui-toolbars
  ui->toolBar->setStyleSheet("QToolButton:!hover {background-color:rgb(199, 223, 197)} QToolBar {background-color:rgb(199, 223, 197)}");
  ui->toolBar_2->setStyleSheet("QToolButton:!hover {background-color:rgb(199, 223, 197)} QToolBar {background-color:rgb(199, 223, 197)}");
  ui->toolBar_3->setStyleSheet("QToolButton:!hover {background-color:rgb(199, 223, 197)} QToolBar {background-color:rgb(199, 223, 197)}");

  // init the object to handle the internal dialogs
  m_dialog_tools = new Labonatip_tools();

  // all the connects are in this function
  initConnects();
  
  m_simulationOnly = ui->actionSimulation->isChecked();

  // status bar to not connected
  ui->statusBar->showMessage("STATUS: NOT Connected  ");

  // hide the warning label
  m_pmap_okIcon = new QPixmap();
  m_pmap_warningIcon = new QPixmap();
  m_pmap_okIcon->load("./icons/okIcon.png");
  m_pmap_warningIcon->load("./icons/warning.png");
  ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
  //ui->label_warningIcon->hide();
  //ui->label_warning->hide();
  ui->label_warning->setText(" ok ! ");

  // move the arrow in the drawing to point at the solution 1
  ui->widget_solutionArrow->setVisible(true);
  ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol1);
  ui->widget_solutionArrow->move(QPoint(400, ui->widget_solutionArrow->pos().ry()));
  //TODO: I don't like the static movement, let's find another solution for it ! 
   
  // set the scene for the graphic depiction of the solution flow
  m_scene_solution = new QGraphicsScene;
  {
	  // set the scene rectagle to avoid the graphic area to move
	  float s_x = 0.0;   // x-coordinate
	  float s_y = 0.0;   // y-coordinate
	  float s_w = 40.0;  // width
	  float s_h = 100.0; // height
	  m_scene_solution->setSceneRect(s_x, s_y, s_w, s_h);
  }
  m_pen_flow.setColor(Qt::transparent);
  m_gradient_flow = new QRadialGradient(c_x, c_y, g_radius);
  m_gradient_flow->setColorAt(0, Qt::lightGray);  // default is from gray
  m_gradient_flow->setColorAt(1, Qt::transparent);        // to white, alpha value 0 to ensure transparency
  m_pen_line.setColor(Qt::lightGray);
  m_pen_line.setWidth(m_pen_line_width);
 

  Log_file_name = QString("./Exp_data/LogFile.dat");

  // initialize PPC1api
  m_ppc1->setCOMport(m_dialog_tools->m_comSettings->name);
  m_ppc1->setBaudRate((int)m_dialog_tools->m_comSettings->baudRate);

  //close the dock tool at inizialization
  //closeOpenDockTools();
  this->resize(QSize(this->minimumWidth(), this->minimumHeight()));


  // init thread macroRunner //TODO: this is just a support, check if needed
  //Labonatip_macroRunner *m_macroRunner_thread = new Labonatip_macroRunner( this );
  m_macroRunner_thread = new Labonatip_macroRunner(this);
  m_macroRunner_thread->setDevice(m_ppc1);
  connect(m_macroRunner_thread, 
	  &Labonatip_macroRunner::resultReady, this, 
	  &Labonatip_GUI::macroFinished); 
  connect(m_macroRunner_thread,
	  &Labonatip_macroRunner::sendStatusMessage, this,
	  &Labonatip_GUI::updateMacroStatusMessage ); 

  // init the timers 
  m_update_time_s1 = new QTimer();
  m_update_time_s2 = new QTimer();
  m_update_time_s3 = new QTimer();
  m_update_time_s4 = new QTimer();
  m_update_GUI = new QTimer();  
  m_timer_solution = 0;

  m_update_time_s1->setInterval(m_base_time_step);
  m_update_time_s2->setInterval(m_base_time_step);
  m_update_time_s3->setInterval(m_base_time_step);
  m_update_time_s4->setInterval(m_base_time_step);
  m_update_GUI->setInterval(m_base_time_step);

  connect(m_update_time_s1, SIGNAL(timeout()), this, SLOT(updateTimingSlider_s1()));
  connect(m_update_time_s2, SIGNAL(timeout()), this, SLOT(updateTimingSlider_s2()));
  connect(m_update_time_s3, SIGNAL(timeout()), this, SLOT(updateTimingSlider_s3()));
  connect(m_update_time_s4, SIGNAL(timeout()), this, SLOT(updateTimingSlider_s4()));
  connect(m_update_GUI, SIGNAL(timeout()), this, SLOT(updateGUI()));

  //set color properties rounded progress bars for solution buttons 
  setGUIbars();

  setGUIchart();
}




void Labonatip_GUI::openFile() {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QString _path = QFileDialog::getOpenFileName (this, tr("Open file"), QDir::currentPath(),  // dialog to open files
						"Something (*.dat);; Binary File (*.bin);; All Files(*.*)" , 0);

//	if (open file function ) 
		QMessageBox::warning(this, "Warning !", "File not found ! <br>" + _path);


   QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

// do something with the new open file
}

void Labonatip_GUI::saveFile() {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	/*if (something->isEmpty()) 
	{QMessageBox::warning(this, "Warning !", "something empty cannot not saved  ! " );
	}*/
	QString _path = QFileDialog::getSaveFileName (this, tr("Save something"), QDir::currentPath(),  // dialog to open files
						"Something (*.dat);; Binary File (*.bin);; All Files(*.*)" , 0);

//	if (save file function ) 
		QMessageBox::warning(this, "Warning !", "File not saved ! <br>" + _path);
  
   QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}


void Labonatip_GUI::showToolsDialog() {

	m_dialog_tools->setWindowFlags(Qt::WindowStaysOnTopHint);
	m_dialog_tools->setModal(false);
	m_macro = new std::vector<fluicell::PPC1api::command>();
	m_dialog_tools->setMacroPrt(m_macro);
	m_dialog_tools->show();

}


void Labonatip_GUI::dropletSizePlus() {
	// only Pon + 2.5%
	// V_recirc - 2.5%
	if (m_pipette_active)	{ 
		if (!m_ppc1->increaseDropletSize())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}

	if (m_simulationOnly) {
		
		if (ui->horizontalSlider_recirculation->value() == 0){
			QMessageBox::information(this, "Warning !", " Recirculation zero, operation cannot be done ");
			return;
		}
		else {
			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 0.975);
		}

		if (ui->horizontalSlider_p_on->value() == 0) {
			ui->horizontalSlider_p_on->setValue(3);
		}
		else{
			ui->horizontalSlider_p_on->setValue(
				ui->horizontalSlider_p_on->value() * 1.025);
		}
		
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::dropletSizeMinus() {
	// only Pon - 2.5%
	// V_recirc + 2.5%
	if (m_pipette_active) {
		if (! m_ppc1->decreaseDropletSize())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}

	if (m_simulationOnly) {

		if (ui->horizontalSlider_p_on->value() == 0) {
			QMessageBox::information(this, "Warning !", " P_on zero, operation cannot be done ");
			return;
		}
		else{
			ui->horizontalSlider_p_on->setValue(
				ui->horizontalSlider_p_on->value() * 0.975);
		}

		if (ui->horizontalSlider_recirculation->value() == 0){
			ui->horizontalSlider_recirculation->setValue(3);
		}
		else{

			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 1.025);
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedPlus() {
	// +5% to all values
		// Poff does not read too low values, 
		// if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	if (m_pipette_active) {
		if (!m_ppc1->increaseFlowspeed())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}
	if (m_simulationOnly) {
		
		if (ui->horizontalSlider_p_on->value() == 0){
			ui->horizontalSlider_p_on->setValue(5);
		}
		else{
			ui->horizontalSlider_p_on->setValue(
				ui->horizontalSlider_p_on->value() * 1.05);
		}

		if (ui->horizontalSlider_p_off->value() == 0){
			ui->horizontalSlider_p_off->setValue(5);
		}
		else{

			ui->horizontalSlider_p_off->setValue(
				ui->horizontalSlider_p_off->value() * 1.05);
		}

		if (ui->horizontalSlider_switch->value() == 0){
			ui->horizontalSlider_switch->setValue(5);
		}
		else{

			ui->horizontalSlider_switch->setValue(
				ui->horizontalSlider_switch->value() * 1.05);
		}

		if (ui->horizontalSlider_recirculation->value() == 0){
			ui->horizontalSlider_recirculation->setValue(5);
		}
		else{

			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 1.05);
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedMinus() {
	// -5% to all values
	if (m_pipette_active) {
		if (!m_ppc1->decreaseFlowspeed())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}
	if (m_simulationOnly) {

		ui->horizontalSlider_p_on->setValue(
			ui->horizontalSlider_p_on->value() * 0.95);

		ui->horizontalSlider_p_off->setValue(
			ui->horizontalSlider_p_off->value() * 0.95);

		ui->horizontalSlider_switch->setValue(
			ui->horizontalSlider_switch->value() * 0.95);

		ui->horizontalSlider_recirculation->setValue(
			ui->horizontalSlider_recirculation->value() * 0.95);

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumPlus() {
	// +5% v_recirculation
	if (m_pipette_active) {
		if (!m_ppc1->increaseVacuum5p())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values.");
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0){
			ui->horizontalSlider_recirculation->setValue(5);
		}
		else{

			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 1.05);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumMinus() {
	// -5% v_recirculation
	if (m_pipette_active) {
		if (!m_ppc1->decreaseVacuum5p())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0){
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Recirculation is zero. ");
		}

		int value = ui->horizontalSlider_recirculation->value() * 0.95;
		ui->horizontalSlider_recirculation->setValue(value);
		
		updateFlowControlPercentages();
	}
}
   
void Labonatip_GUI::updateFlowControlPercentages()
{
	if (m_simulationOnly) {
	
		{
			int pon = 100 * 
				ui->horizontalSlider_p_on->value() / 
				m_dialog_tools->m_pr_params->p_on_default; // TODO direct access the public member, add get function instead

			int vr = 100 * 
				ui->horizontalSlider_recirculation->value() / 
				m_dialog_tools->m_pr_params->v_recirc_default;  // TODO direct access the public member, add get function instead

			int droplet_percentage = (pon + vr) / 2;
			ui->lcdNumber_dropletSize_percentage->display(droplet_percentage);
		}
		{
			int pon = 100 * 
				ui->horizontalSlider_p_on->value() / 
				m_dialog_tools->m_pr_params->p_on_default; // TODO direct access the public member, add get function instead

			int poff = 100 * 
				ui->horizontalSlider_p_off->value() / 
				m_dialog_tools->m_pr_params->p_off_default; // TODO direct access the public member, add get function instead;
			
			int vs = 100 * 
				ui->horizontalSlider_switch->value() / 
				m_dialog_tools->m_pr_params->v_switch_default; // TODO direct access the public member, add get function instead;

			int vr = 100 * 
				ui->horizontalSlider_recirculation->value() / 
				m_dialog_tools->m_pr_params->v_recirc_default; // TODO direct access the public member, add get function instead;
			
			int flowspeed_percentage = (pon + poff + vs + vr) / 4; // 4 is for the average
			ui->lcdNumber_flowspeed_percentage->display(flowspeed_percentage);
		}

		{
			int vacuum_percentage = 100 * 
				ui->horizontalSlider_recirculation->value() / 
				m_dialog_tools->m_pr_params->v_recirc_default; // TODO direct access the public member, add get function instead;

			ui->lcdNumber_vacuum_percentage->display(vacuum_percentage);
		}
	}

}

void Labonatip_GUI::pressurePonDown() {
	ui->horizontalSlider_p_on->setValue(ui->horizontalSlider_p_on->value() - 1);
}

void Labonatip_GUI::pressurePonUp()	{
	ui->horizontalSlider_p_on->setValue(ui->horizontalSlider_p_on->value() + 1);
}

void Labonatip_GUI::pressurePoffDown()	{
	   ui->horizontalSlider_p_off->setValue(ui->horizontalSlider_p_off->value() - 1);
}
   
void Labonatip_GUI::pressurePoffUp()	{
	   ui->horizontalSlider_p_off->setValue(ui->horizontalSlider_p_off->value() + 1);
}

 void Labonatip_GUI::pressButtonPressed_switchDown() {
	 ui->horizontalSlider_switch->setValue(ui->horizontalSlider_switch->value() - 1);
}

 void Labonatip_GUI::pressButtonPressed_switchUp()	{
	 ui->horizontalSlider_switch->setValue(ui->horizontalSlider_switch->value() + 1);
}
 
void Labonatip_GUI::recirculationDown() {
	ui->horizontalSlider_recirculation->setValue(ui->horizontalSlider_recirculation->value() - 1);
}
   
void Labonatip_GUI::recirculationUp() {
	   ui->horizontalSlider_recirculation->setValue(ui->horizontalSlider_recirculation->value() + 1);
}

void Labonatip_GUI::reboot() {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	if (m_pipette_active) {
		disCon(); // with the pipette active this will stop the threads
		ui->actionSimulation->setChecked(false);

		int counter = 200;
		QProgressDialog *PD = new QProgressDialog("Rebooting ...", "Cancel", 0, counter, this);
		PD->show();
		PD->setWindowModality(Qt::WindowModal);
		m_ppc1->reboot();

		ui->horizontalSlider_p_on->setValue(0);
		ui->horizontalSlider_p_off->setValue(0);
		ui->horizontalSlider_recirculation->setValue(0);
		ui->horizontalSlider_switch->setValue(0);
		for (int i = 0; i < counter; i++) {
			PD->setValue(i);
            std::this_thread::sleep_for(std::chrono::microseconds(100000));  //--> wait the last execution
			//if (PD->wasCanceled()) // the operation cannot be cancelled
				//QMessageBox::information(this, "Warning !", " Reboot cannot be canceled  ");
		}
		PD->cancel();

		m_ppc1->connectCOM();
        std::this_thread::sleep_for(std::chrono::microseconds(500000));  //--> wait the last execution

		disCon();
	}
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::disCon() {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

    if (m_simulationOnly) 	{ 
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::information(this, "Warning !", "Lab-on-a-tip is in simulation only  ");
		return;
	}

	if (!m_ppc1->isRunning()) {

		if (!m_ppc1->isConnected())
			if (!m_ppc1->connectCOM()) { // TODO: this is not good !! this should not be connected always
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

		m_ppc1->run();
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

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}

void Labonatip_GUI::operationalMode()  {

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	// allOff()
	// setPoff(21)
	// setPon(190)
	// setVswitch(-115)
	// setVrecirc(-115)
	if (m_pipette_active) {
		if(m_ppc1->isConnected()) m_ppc1->pumpingOff();
	}
	ui->horizontalSlider_switch->setValue(0);  // on slider changed, it send the value to the pumps
	ui->horizontalSlider_recirculation->setValue(0);
	ui->horizontalSlider_p_on->setValue(0);
	ui->horizontalSlider_p_off->setValue(0);
	QThread::msleep(1000);

	// if the sliders were in the same position, it does not change !!! 
	// so the event slider_changed is not emitted
	ui->horizontalSlider_switch->setValue(115);  // on slider changed, it send the value to the pumps
	ui->horizontalSlider_recirculation->setValue(115);
	ui->horizontalSlider_p_on->setValue(190);
	ui->horizontalSlider_p_off->setValue(21);

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::runMacro() {

	if (!m_macroRunner_thread->isRunning()) {

		m_macroRunner_thread->setMacroPrt(m_macro);

		m_macroRunner_thread->setSimulationFlag(m_simulationOnly);
		m_macroRunner_thread->start();
		
		ui->groupBox_deliveryZone->setEnabled(false);
		ui->pushButton_operational->setEnabled(false);
		ui->pushButton_Initialize->setEnabled(false); 
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
		ui->pushButton_Initialize->setEnabled(true);
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

void Labonatip_GUI::macroFinished(const QString &_result) {

	QMessageBox::information(this, " Information !", _result);
	ui->statusBar->showMessage("MACRO FINISHED");
	ui->label_runMacro->setText("Run Macro");
	ui->groupBox_deliveryZone->setEnabled(true);
	ui->pushButton_operational->setEnabled(true);
	ui->pushButton_Initialize->setEnabled(true);
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

void Labonatip_GUI::updateMacroStatusMessage(const QString &_message) {

	QString s = " MACRO RUNNING :: status message >>  ";
	s.append(_message);
	ui->statusBar->showMessage(s);
}

void Labonatip_GUI::pumpingOff() {

	if (m_pipette_active) {
		m_ppc1->pumpingOff();
	}

	ui->horizontalSlider_p_on->setValue(0);
	ui->horizontalSlider_p_off->setValue(0);
	ui->horizontalSlider_recirculation->setValue(0);
	ui->horizontalSlider_switch->setValue(0);

	//this will stop the solution flow 
	m_timer_solution = std::numeric_limits<int>::max();

	// reset the wells
	resetWells();
}


void Labonatip_GUI::shutdown() {

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
	//OLD SLEEP MACRO
	// allOff()
	// setPoff(11)
	// setPon(0)
	// sleep(5)
	// setVswitch(-45)
	// setVrecirc(-45)
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	if (m_pipette_active) {
		if (m_ppc1->isConnected()) m_ppc1->pumpingOff();

		ui->horizontalSlider_p_on->setValue(0);
		ui->horizontalSlider_p_off->setValue(11);
		
		QThread::sleep(5);
		ui->horizontalSlider_switch->setValue(45);
		ui->horizontalSlider_recirculation->setValue(45);
	}  

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::allOff() {
	if (m_pipette_active) m_ppc1->closeAllValves();
}

void Labonatip_GUI::setEnableSolutionButtons(bool _enable ) {
	ui->pushButton_solution1->setEnabled(_enable);
	ui->pushButton_solution2->setEnabled(_enable);
	ui->pushButton_solution3->setEnabled(_enable);
	ui->pushButton_solution4->setEnabled(_enable);
}


void Labonatip_GUI::closeOpenDockTools() {

	if (!ui->dockWidget->isHidden()) {
		ui->dockWidget->hide();
		if (!this->isMaximized())
			this->resize(QSize(this->minimumWidth(), this->height()));
	}
	else {
		ui->dockWidget->show();
		if (!this->isMaximized())
			this->resize(QSize(this->minimumWidth() + ui->dockWidget->width(), this->height()));
	}

}

void Labonatip_GUI::updateDrawing( int _value) {

    //clean the scene
	m_scene_solution->clear();

	_value = _value / 2;
	// draw the circle 
	QBrush brush(*m_gradient_flow);
	m_scene_solution->addEllipse( c_x, c_y - _value/2, 
		                          c_radius + _value, c_radius + _value, 
		                          m_pen_flow, brush ); 
	
	// draw a line from the injector to the solution release point 
	m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);

	ui->graphicsView->setScene(m_scene_solution);
	ui->graphicsView->show();
	return;
}


void Labonatip_GUI::pushSolution1()
{

	if (!ui->pushButton_solution1->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_time_s1->stop();

		return;
	}

	ui->pushButton_solution2->setChecked(false);
	pushSolution2(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution3->setChecked(false);
	pushSolution3(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution4->setChecked(false);
	pushSolution4(); // this will actually stop the flow as the check state is false

	// set the color into the drawing to fit the solution flow 
	m_gradient_flow->setColorAt(0, m_sol1_color);  // from dark green 
	m_gradient_flow->setColorAt(1, Qt::white); // to white, alpha value 0 to ensure transparency
	m_pen_line.setColor(m_sol1_color);

	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	else updateDrawing(ui->horizontalSlider_p_on->value());

	// move the arrow in the drawing to point on the solution 1
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol1);
	ui->widget_solutionArrow->move(QPoint(400, ui->widget_solutionArrow->pos().ry()));
	//TODO: I don't like the static movement, let's find another solution for it ! 

	// Here start the solution flow ---TODO: the connection to the real device is still missing
	m_time_multipilcator = (int)(ui->doubleSpinBox_solution->value() );

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_a(true);
	}
	m_timer_solution = 0;
	m_update_time_s1->start();

//	setEnableSolutionButtons(false);

}

void Labonatip_GUI::pushSolution2() {

	if (!ui->pushButton_solution2->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_time_s2->stop();

		return;
	}
	
	ui->pushButton_solution1->setChecked(false);
	pushSolution1(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution3->setChecked(false);
	pushSolution3(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution4->setChecked(false);
	pushSolution4(); // this will actually stop the flow as the check state is false

	// set the color into the drawing to fit the solution flow 
	m_gradient_flow->setColorAt(0, m_sol2_color);   // from light green 
	m_gradient_flow->setColorAt(1, Qt::white); // to white, alpha value 0 to ensure transparency
	m_pen_line.setColor(m_sol2_color);
	
	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	else updateDrawing(ui->horizontalSlider_p_on->value());

	// move the arrow in the drawing to point on the solution 2
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol2);
	ui->widget_solutionArrow->move(QPoint(345, ui->widget_solutionArrow->pos().ry()));
	//TODO: I don't like the static movement, let's find another solution for it ! 

	// Here start the solution flow ---TODO: the connection to the real device is still missing
	//m_time_multipilcator_s2 = (int)(ui->doubleSpinBox_solution->value() );
	m_time_multipilcator = (int)(ui->doubleSpinBox_solution->value());

	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_b(true);
	}

	m_timer_solution = 0;
	m_update_time_s2->start();
	//setEnableSolutionButtons(false);

}

void Labonatip_GUI::pushSolution3() {

	if (!ui->pushButton_solution3->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_time_s3->stop();

		return;
	}

	ui->pushButton_solution1->setChecked(false);
	pushSolution1(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution2->setChecked(false);
	pushSolution2(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution4->setChecked(false);
	pushSolution4(); // this will actually stop the flow as the check state is false

	// set the color into the drawing to fit the solution flow 
	m_gradient_flow->setColorAt(0, m_sol3_color);  // from orange
	m_gradient_flow->setColorAt(1, Qt::white);   // to white, alpha value 0 to ensure transparency
	m_pen_line.setColor(m_sol3_color);

	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	else updateDrawing(ui->horizontalSlider_p_on->value());

	// move the arrow in the drawing to point on the solution 3
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol3);
	ui->widget_solutionArrow->move(QPoint(290, ui->widget_solutionArrow->pos().ry()));
	//TODO: I don't like the static movement, let's find another solution for it ! 

	// Here start the solution flow ---TODO: the connection to the real device is still missing
	//m_time_multipilcator_s3 = (int)(ui->doubleSpinBox_solution->value() );
	m_time_multipilcator = (int)(ui->doubleSpinBox_solution->value());

	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_c(true);
	}

	m_timer_solution = 0;
	m_update_time_s3->start();
	//setEnableSolutionButtons(false);

}

void Labonatip_GUI::pushSolution4() {

	if (!ui->pushButton_solution4->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_time_s4->stop();

		return;
	}

	ui->pushButton_solution1->setChecked(false);
	pushSolution1(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution2->setChecked(false);
	pushSolution2(); // this will actually stop the flow as the check state is false
	ui->pushButton_solution3->setChecked(false);
	pushSolution3(); // this will actually stop the flow as the check state is false

	// set the color into the drawing to fit the solution flow 
	m_gradient_flow->setColorAt(0.0, m_sol4_color);  // from solution 4 preset color
	m_gradient_flow->setColorAt(1.0, Qt::white); // to white, alpha value 0 to ensure transparency
	m_pen_line.setColor(m_sol4_color);

	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	else updateDrawing(ui->horizontalSlider_p_on->value());

	// move the arrow in the drawing to point on the solution 4
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol4);
	ui->widget_solutionArrow->move(QPoint(235, ui->widget_solutionArrow->pos().ry()));
	//TODO: I don't like the static movement, let's find another solution for it ! 

	// Here start the solution flow ---TODO: the connection to the real device is still missing
	//m_time_multipilcator_s4 = (int)(ui->doubleSpinBox_solution->value() );
	m_time_multipilcator = (int)(ui->doubleSpinBox_solution->value());

	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_d(true);
	}

	m_timer_solution = 0;
	m_update_time_s4->start();
	//setEnableSolutionButtons(false);

}


void Labonatip_GUI::resetWells() {

	ui->progressBar_solution1->setValue(100);
	ui->progressBar_solution2->setValue(100);
	ui->progressBar_solution3->setValue(100);
	ui->progressBar_solution4->setValue(100);
}


void Labonatip_GUI::sliderPonChanged(int _value)  {

	if(m_simulationOnly) {
		ui->label_PonPressure->setText(QString(QString::number(_value) + " mbar"));
	    ui->progressBar_pressure_p_on->setValue(_value); 
		updateFlowControlPercentages();
	}

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledPon->setValue(0); // turn the led off
		return;
	}
	ui->progressBar_ledPon->setValue(m_ppc1->m_PPC1_data->channel_D->state); // turn the led on

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) 	{
		m_ppc1->setPressureChannelD(_value);
	}

}


void Labonatip_GUI::sliderPoffChanged(int _value) {

	if (m_simulationOnly) {
		ui->label_PoffPressure->setText(QString(QString::number(_value) + " mbar"));
		ui->progressBar_pressure_p_off->setValue(_value);
		updateFlowControlPercentages();
	}

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledPoff->setValue(0); // turn the led off
		return;
	}
	ui->progressBar_ledPoff->setValue(m_ppc1->m_PPC1_data->channel_C->state); // turn the led on

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active)	{
		m_ppc1->setPressureChannelC(_value);
	}

}


void Labonatip_GUI::sliderRecircChanged(int _value) {

	if (m_simulationOnly) {
		ui->label_recircPressure->setText(QString(QString::number(-_value) + " mbar"));
		ui->progressBar_recirc->setValue(_value);
		updateFlowControlPercentages();
	}

	ui->progressBar_recircIn->setValue(_value);
	ui->progressBar_recircOut->setValue(_value);

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledRecirc->setValue(0);  // turn the led off
		return;
	}

	ui->progressBar_ledRecirc->setValue(m_ppc1->m_PPC1_data->channel_A->state); // turn the led on

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) 	{
		m_ppc1->setVacuumChannelA(-_value);
	}

}


void Labonatip_GUI::sliderSwitchChanged(int _value) {
	
	if (m_simulationOnly) {
		ui->label_switchPressure->setText(QString(QString::number(-_value) + " mbar"));
		ui->progressBar_switch->setValue(_value);
		updateFlowControlPercentages();
	}

	ui->progressBar_switchIn->setValue(_value);
	ui->progressBar_switchOut->setValue(_value);

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledSwitch->setValue(0);
		return;
	}

	ui->progressBar_ledSwitch->setValue(m_ppc1->m_PPC1_data->channel_B->state);

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) 	{
		m_ppc1->setVacuumChannelB(-_value);
	}

}

void Labonatip_GUI::updateTimingSlider_s1() {

	if (m_timer_solution < m_time_multipilcator) {
		m_update_time_s1->start();
		int status = int(100 * m_timer_solution / m_time_multipilcator);
		//ui->widget_sol1->setValue(status);
		ui->progressBar_solution1->setValue(100 - status);
		QString s;
		s.append("Empty in ");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution) ;
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, ");
		s.append(QString::number(remaining_mins));
		s.append(" min ");
		ui->label_emptyTime->setText(s);
		ui->doubleSpinBox_solution->setValue(remaining_time_in_sec);
		m_timer_solution++;
		// show the warning label

		//ui->label_warningIcon->show();
		//ui->label_warning->show();
		if (status > 50) {
			ui->label_warningIcon->setPixmap(*m_pmap_warningIcon);
			ui->label_warning->setText(" warning solution is ending ");
		}
		return;
	}
	else 
	{
		m_update_time_s1->stop();
		m_timer_solution = 0;
		ui->progressBar_solution1->setValue(0);
		//ui->widget_sol1->setValue(100);
		if (m_pipette_active)
		{
			m_ppc1->setValve_a(false);
		}
		setEnableSolutionButtons(true);
		ui->pushButton_solution1->setChecked(false);

		ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
		ui->label_warning->setText(" Ok! ");
		return;
	}

}

void Labonatip_GUI::updateTimingSlider_s2() {

	if (m_timer_solution < m_time_multipilcator) {
		m_update_time_s2->start();
		int status = int(100 * m_timer_solution / m_time_multipilcator);
		//ui->widget_sol2->setValue(status);
		ui->progressBar_solution2->setValue(100 - status);
		QString s;
		s.append("Empty in ");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, ");
		s.append(QString::number(remaining_mins));
		s.append(" min ");
		ui->label_emptyTime->setText(s);
		ui->doubleSpinBox_solution->setValue(remaining_time_in_sec);
		m_timer_solution++;
		if (status > 50) {
			ui->label_warningIcon->setPixmap(*m_pmap_warningIcon);
			ui->label_warning->setText(" warning solution is ending ");
		}
		return;
	}
	else
	{
		m_update_time_s2->stop();
		m_timer_solution = 0;
		ui->progressBar_solution2->setValue(0);
		//ui->widget_sol2->setValue(100);
		if (m_pipette_active)
		{
			m_ppc1->setValve_b(false);
		}
		setEnableSolutionButtons(true);
		ui->pushButton_solution2->setChecked(false);

		ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
		ui->label_warning->setText(" Ok! ");
		return;
	}

}

void Labonatip_GUI::updateTimingSlider_s3() {

	if (m_timer_solution < m_time_multipilcator) {
		m_update_time_s3->start();
		int status = int(100 * m_timer_solution / m_time_multipilcator);
		//ui->widget_sol3->setValue(status);
		ui->progressBar_solution3->setValue(100 - status);
		QString s;
		s.append("Empty in ");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution) ;
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, ");
		s.append(QString::number(remaining_mins));
		s.append(" min ");
		ui->label_emptyTime->setText(s);
		ui->doubleSpinBox_solution->setValue(remaining_time_in_sec);
		m_timer_solution++;
		if (status > 50) {
			ui->label_warningIcon->setPixmap(*m_pmap_warningIcon);
			ui->label_warning->setText(" warning solution is ending ");
		}
		return;
	}
	else
	{
		m_update_time_s3->stop();
		m_timer_solution = 0;
		ui->progressBar_solution3->setValue(0);
		//ui->widget_sol3->setValue(100);
		if (m_pipette_active)
		{
			m_ppc1->setValve_c(false);
		}
		setEnableSolutionButtons(true);
		ui->pushButton_solution3->setChecked(false);

		ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
		ui->label_warning->setText(" Ok! ");
		return;
	}

}

void Labonatip_GUI::updateTimingSlider_s4() {

	if (m_timer_solution < m_time_multipilcator) {
		m_update_time_s4->start();
		int status = int(100 * m_timer_solution / m_time_multipilcator);
		//ui->widget_sol4->setValue(status);
		ui->progressBar_solution4->setValue(100 - status);
		QString s;
		s.append("Empty in ");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, ");
		s.append(QString::number(remaining_mins));
		s.append(" min ");
		ui->label_emptyTime->setText(s);
		ui->doubleSpinBox_solution->setValue(remaining_time_in_sec);
		m_timer_solution++;
		if (status > 50) {
			ui->label_warningIcon->setPixmap(*m_pmap_warningIcon);
			ui->label_warning->setText(" warning solution is ending ");
		}
		return;
	}
	else
	{
		m_update_time_s4->stop();
		m_timer_solution = 0;
		ui->progressBar_solution4->setValue(0);
		//ui->widget_sol4->setValue(100);
		if (m_pipette_active)
		{
			m_ppc1->setValve_d(false);
		}
		setEnableSolutionButtons(true);
		ui->pushButton_solution4->setChecked(false);

		ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
		ui->label_warning->setText(" Ok! ");
		return;
	}

}

void Labonatip_GUI::updateGUI() {
	float value = roundf(m_ppc1->m_PPC1_data->channel_B->sensor_reading * 100) / 100;  // rounded to second decimal
	ui->label_switchPressure->setText(QString(QString::number(value) + " mbar"));
	ui->progressBar_switch->setValue(-value);
	//ui->horizontalSlider_switch->setValue(-value);

	value = roundf(m_ppc1->m_PPC1_data->channel_A->sensor_reading * 100) / 100;
	ui->label_recircPressure->setText(QString(QString::number(value) + " mbar")); 
	ui->progressBar_recirc->setValue(-value);
	//ui->horizontalSlider_recirculation->setValue(-value);

	value = roundf(m_ppc1->m_PPC1_data->channel_C->sensor_reading * 100) / 100;
	ui->label_PoffPressure->setText(QString(QString::number(value) + " mbar"));
	ui->progressBar_pressure_p_off->setValue(value);
	//ui->horizontalSlider_1->setValue(value);

	value = roundf(m_ppc1->m_PPC1_data->channel_D->sensor_reading * 100) / 100;
	ui->label_PonPressure->setText(QString(QString::number(value) + " mbar"));
	ui->progressBar_pressure_p_on->setValue(value);
	//ui->horizontalSlider_2->setValue(value);

	ui->lcdNumber_dropletSize_percentage->display(m_ppc1->getDropletSizePercentage());
	ui->progressBar_dropletSize->setValue(m_ppc1->getDropletSizePercentage());
	ui->lcdNumber_flowspeed_percentage->display(m_ppc1->getFlowSpeedPercentage());
	ui->progressBar_flowSpeed->setValue(m_ppc1->getFlowSpeedPercentage());
	ui->lcdNumber_vacuum_percentage->display(m_ppc1->getVacuumPercentage());
	ui->progressBar_vacuum->setValue(m_ppc1->getVacuumPercentage());

	updateDrawing(m_ppc1->getDropletSizePercentage());

	if (m_ppc1->isRunning())
  	    m_update_GUI->start();
}


bool Labonatip_GUI::saveLog(QString &_file_name)
{
	// data stream interface
	QFile _file(_file_name);
	QTextStream out(&_file);
	out.setCodec("UTF-8");

	if (!QFile::exists(_file_name))
	{
		QMessageBox::StandardButton message;
		message = QMessageBox::question(this, "Message", 
			      "File " + _file.fileName() + " does not exists. \n Do you want to create it?",
			      QMessageBox::Yes | QMessageBox::No);
		if (message == QMessageBox::Yes) {
			// create the file
			Log_file_name = QString("./Ext_data/LogFile.dat");
			_file.setFileName(Log_file_name);
			if (!_file.open(QIODevice::Append | QIODevice::Text)) {
				//file not created, return false
				QMessageBox::information(this, "Error !", " Could not open the file " + _file.fileName() );
				return false;
			}

			// add the header
			out << "%% LogFile Header V. 0.1 " << endl;
			out << "%% File created on " << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") << endl;
			out << "%% This file logs the Fluicel Lab-on-a-tip parameters GUI demo - Mauro Bellone " << endl;
			out << "%% The saved data are:: " << endl;
			out << "%% value1 in [UNIT] " << endl;

			// write data

			out << "DATA TO WRITE" << endl;

			// close the file
			_file.close();
		}
		else {
			//file not created, return false
			QMessageBox::information(this, "Warning !", "  File was not created  ");
			return false;
		}
	}
	else {
		if (_file.open(QIODevice::Append | QIODevice::Text)) { 
			out << "DATA TO WRITE" << endl; 
			_file.close();
			return true;
		}
		else {
			//file cannot be open, return false
			QMessageBox::information(this, "Error !", " Could not open the file " + _file.fileName());
			return false;
		}
		
	}
	//TODO: there is no check on the folder existance or path validity!!!
	//if (!m_saveFolderPath.exists()) //create the folder! e.g. ./Ext_data
	//TODO: check for permission and actual possibility to write files in a folder

	return true;
};



void Labonatip_GUI::initConnects()
{

	//windows toolbar
	connect(ui->actionTools, SIGNAL(triggered()), this, SLOT(showToolsDialog()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionNerdy, SIGNAL(triggered()), this, SLOT(closeOpenDockTools()));
	connect(ui->actionDisCon, SIGNAL(triggered()), this, SLOT(disCon()));
	connect(ui->actionSimulation, SIGNAL(triggered()), this, SLOT(simulationOnly()));
	//connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(runOperations()));
	connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(reboot()));
	//connect(ui->actionSleep, SIGNAL(triggered()), this, SLOT(pumpingOff()));
	connect(ui->actionShutdown, SIGNAL(triggered()), this, SLOT(shutdown()));
	connect(ui->actionWhatsthis, SIGNAL(triggered()), this, SLOT(ewst()));

	// connect buttons
	connect(ui->pushButton_p_on_down, SIGNAL(clicked()), this, SLOT(pressurePonDown()));
	connect(ui->pushButton_p_on_up, SIGNAL(clicked()), this, SLOT(pressurePonUp()));
	connect(ui->pushButton_p_off_down, SIGNAL(clicked()), this, SLOT(pressurePoffDown()));
	connect(ui->pushButton_p_off_up, SIGNAL(clicked()), this, SLOT(pressurePoffUp()));
	connect(ui->pushButton_switchDown, SIGNAL(clicked()), this, SLOT(pressButtonPressed_switchDown()));
	connect(ui->pushButton_switchUp, SIGNAL(clicked()), this, SLOT(pressButtonPressed_switchUp()));
	connect(ui->pushButton_recirculationDown, SIGNAL(clicked()), this, SLOT(recirculationDown()));
	connect(ui->pushButton_recirculationUp, SIGNAL(clicked()), this, SLOT(recirculationUp()));
	connect(ui->pushButton_solution1, SIGNAL(clicked()), this, SLOT(pushSolution1()));
	connect(ui->pushButton_solution2, SIGNAL(clicked()), this, SLOT(pushSolution2()));
	connect(ui->pushButton_solution3, SIGNAL(clicked()), this, SLOT(pushSolution3()));
	connect(ui->pushButton_solution4, SIGNAL(clicked()), this, SLOT(pushSolution4()));
	connect(ui->pushButton_dropSize_minus, SIGNAL(clicked()), this, SLOT(dropletSizeMinus()));
	connect(ui->pushButton_dropSize_plus, SIGNAL(clicked()), this, SLOT(dropletSizePlus()));
	connect(ui->pushButton_flowspeed_minus, SIGNAL(clicked()), this, SLOT(flowSpeedMinus()));
	connect(ui->pushButton_flowspeed_plus, SIGNAL(clicked()), this, SLOT(flowSpeedPlus()));
	connect(ui->pushButton_vacuum_minus, SIGNAL(clicked()), this, SLOT(vacuumMinus()));
	connect(ui->pushButton_vacuum_plus, SIGNAL(clicked()), this, SLOT(vacuumPlus()));
	
	connect(ui->pushButton_standby, SIGNAL(clicked()), this, SLOT(standby()));
	connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(pumpingOff()));
	connect(ui->pushButton_operational, SIGNAL(clicked()), this, SLOT(operationalMode()));
	connect(ui->pushButton_runMacro, SIGNAL(clicked()), this, SLOT(runMacro()));

	
	
	// connect sliders
	connect(ui->horizontalSlider_p_on, SIGNAL(valueChanged(int)), this, SLOT(sliderPonChanged(int)));
	connect(ui->horizontalSlider_p_off, SIGNAL(valueChanged(int)), this, SLOT(sliderPoffChanged(int)));
	connect(ui->horizontalSlider_recirculation, SIGNAL(valueChanged(int)), this, SLOT(sliderRecircChanged(int)));
	connect(ui->horizontalSlider_switch, SIGNAL(valueChanged(int)), this, SLOT(sliderSwitchChanged(int)));
	
	connect(m_dialog_tools, SIGNAL(ok()), this, SLOT(toolOk()));


}

void Labonatip_GUI::toolOk() {

	cout << " ok to the tools window pressed" << endl;
	m_ppc1->setCOMport(m_dialog_tools->m_comSettings->name);
	m_ppc1->setBaudRate((int)m_dialog_tools->m_comSettings->baudRate);

}

void Labonatip_GUI::setGUIbars() {

	QPalette p1;
	p1.setBrush(QPalette::AlternateBase, Qt::black);
	p1.setColor(QPalette::Text, Qt::yellow);
	QPalette p2(p1);
	p2.setBrush(QPalette::Base, Qt::transparent);
	p2.setColor(QPalette::Text, Qt::transparent);
	p2.setColor(QPalette::Shadow, Qt::transparent);

	// set a gradient for solution 1
	QGradientStops gradientPoints_sol1;
	gradientPoints_sol1 << QGradientStop(0.0, m_sol1_color)
		<< QGradientStop(0.5, m_sol1_color_g1)
		<< QGradientStop(1.0, m_sol1_color_g1);
	// set a gradient for solution 2
	QGradientStops gradientPoints_sol2;
	gradientPoints_sol2 << QGradientStop(0.0, m_sol2_color)
		<< QGradientStop(0.5, m_sol2_color_g1)
		<< QGradientStop(1.0, m_sol2_color_g1);
	// set a gradient for solution 3
	QGradientStops gradientPoints_sol3;
	gradientPoints_sol3 << QGradientStop(0.0, m_sol3_color)
		<< QGradientStop(0.5, m_sol3_color_g05)
		<< QGradientStop(1.0, m_sol3_color_g1);
	// set a gradient for solution 4
	QGradientStops gradientPoints_sol4;
	gradientPoints_sol4 << QGradientStop(0.0, m_sol4_color)
		<< QGradientStop(0.5, m_sol4_color_g05)
		<< QGradientStop(1.0, m_sol4_color_g1);

	// set the round progress bars for solution 1
	ui->widget_sol1->setFormat("%p");
	ui->widget_sol1->setDecimals(0);
	ui->widget_sol1->setPalette(p2);
	ui->widget_sol1->setBarStyle(QRoundProgressBar::StylePie);
	ui->widget_sol1->setOutlinePenWidth(0);
	ui->widget_sol1->setDataPenWidth(0);
	ui->widget_sol1->setDataColors(gradientPoints_sol1);
	//ui->widget_sol1->setRange(ui->horizontalSlider_1->minimum(), ui->horizontalSlider_1->maximum());
	ui->widget_sol1->setRange(0, 100);
	ui->widget_sol1->setValue(ui->horizontalSlider_p_on->value());

	// set the round progress bars for solution 2
	ui->widget_sol2->setFormat("%p");
	ui->widget_sol2->setDecimals(0);
	ui->widget_sol2->setPalette(p2);
	ui->widget_sol2->setBarStyle(QRoundProgressBar::StylePie);
	ui->widget_sol2->setOutlinePenWidth(0);
	ui->widget_sol2->setDataPenWidth(0);
	ui->widget_sol2->setDataColors(gradientPoints_sol2);
	//ui->widget_sol2->setRange(ui->horizontalSlider_1->minimum(), ui->horizontalSlider_1->maximum());
	ui->widget_sol4->setRange(0, 100); 
	ui->widget_sol2->setValue(ui->horizontalSlider_p_on->value());

	// set the round progress bars for solution 3
	ui->widget_sol3->setFormat("%p");
	ui->widget_sol3->setDecimals(0);
	ui->widget_sol3->setPalette(p2);
	ui->widget_sol3->setBarStyle(QRoundProgressBar::StylePie);
	ui->widget_sol3->setOutlinePenWidth(0);
	ui->widget_sol3->setDataPenWidth(0);
	ui->widget_sol3->setDataColors(gradientPoints_sol3);
	//ui->widget_sol3->setRange(ui->horizontalSlider_1->minimum(), ui->horizontalSlider_1->maximum());
	ui->widget_sol3->setRange(0, 100);
	ui->widget_sol3->setValue(ui->horizontalSlider_p_on->value());

	// set the round progress bars for solution 4
	ui->widget_sol4->setFormat("%p");
	ui->widget_sol4->setDecimals(0);
	ui->widget_sol4->setPalette(p2);
	ui->widget_sol4->setBarStyle(QRoundProgressBar::StylePie);
	ui->widget_sol4->setOutlinePenWidth(0);
	ui->widget_sol4->setDataPenWidth(0);
	ui->widget_sol4->setDataColors(gradientPoints_sol4);
	//ui->widget_sol4->setRange(ui->horizontalSlider_1->minimum(), ui->horizontalSlider_1->maximum());
	ui->widget_sol4->setRange(0, 100);
	ui->widget_sol4->setValue(ui->horizontalSlider_p_on->value());

}

void Labonatip_GUI::setGUIchart()
{
	cout << " setting GUI charts " << endl;
	//![1]
	m_series_X = new QtCharts::QLineSeries();
	m_series_Y = new QtCharts::QLineSeries();
	m_series_Pon = new QtCharts::QLineSeries();
	m_series_Poff = new QtCharts::QLineSeries();
	m_series_V_switch = new QtCharts::QLineSeries();
	m_series_V_recirc = new QtCharts::QLineSeries();
	m_series_solution = new QtCharts::QLineSeries();
	m_series_ask = new QtCharts::QLineSeries();
	m_series_sync_in = new QtCharts::QLineSeries();
	m_series_sync_out = new QtCharts::QLineSeries();

	cout << " variable initialized " << endl;
	//![1]

	//![2]
	*m_series_X << QPointF(0.0, 0.0) << QPointF(10.0, 0.0) << QPointF(20.0, 0.0) << QPointF(30.0, 0.0) << QPointF(40.0, 0.0)
		<< QPointF(50.0, 0.0) << QPointF(60.0, 0.0) << QPointF(70.0, 0.0) << QPointF(80.0, 0.0) << QPointF(90.0, 0.0) << QPointF(100.0, 0.0);
	*m_series_Y << QPointF(0.0, 0.0) << QPointF(0.0, 10.0) << QPointF(0.0, 20.0) << QPointF(0.0, 30.0) << QPointF(0.0, 40.0)
		<< QPointF(0.0, 50.0) << QPointF(0.0, 60.0) << QPointF(0.0, 70.0) << QPointF(0.0, 80.0) << QPointF(0.0, 90.0) << QPointF(0.0, 100.0);

	*m_series_Pon << QPointF(0.0, 90.0) << QPointF(20.0, 90.0) << QPointF(20.0, 100.0) << QPointF(50.0, 100.0) << QPointF(50.0, 90.0)
		<< QPointF(70.0, 90.0) << QPointF(70.0, 100.0) << QPointF(100.0, 100.0);
	*m_series_Poff << QPointF(0.0, 80.0) << QPointF(30.0, 80.0) << QPointF(30.0, 90.0) << QPointF(40.0, 90.0) << QPointF(40.0, 80.0)
		<< QPointF(70.0, 80.0) << QPointF(70.0, 90.0) << QPointF(100.0, 90.0);
	*m_series_V_switch << QPointF(0.0, 65.0) << QPointF(100.0, 65.0);

	*m_series_V_recirc << QPointF(0.0, 55.0) << QPointF(100.0, 55.0);

	*m_series_solution << QPointF(0.0, 45.0) << QPointF(100.0, 45.0);

	*m_series_ask << QPointF(0.0, 20.0) << QPointF(20.0, 20.0) << QPointF(20.0, 30.0) << QPointF(50.0, 30.0) << QPointF(50.0, 20.0)
		<< QPointF(70.0, 20.0) << QPointF(70.0, 30.0) << QPointF(100.0, 30.0);

	*m_series_sync_in << QPointF(0.0, 10.0) << QPointF(30.0, 10.0) << QPointF(30.0, 20.0) << QPointF(40.0, 20.0) << QPointF(40.0, 20.0)
		<< QPointF(70.0, 20.0) << QPointF(70.0, 10.0) << QPointF(100.0, 10.0);
	*m_series_sync_out << QPointF(0.0, 0.0) << QPointF(20.0, 0.0) << QPointF(20.0, 10.0) << QPointF(50.0, 10.0) << QPointF(50.0, 0.0)
		<< QPointF(70.0, 00.0) << QPointF(70.0, 10.0) << QPointF(100.0, 10.0);

	cout << " Build sample series " << endl;

	//![2]

	//![3]
	//QtCharts::QAreaSeries *series1A = new QtCharts::QAreaSeries(series0, series1);
	//QtCharts::QAreaSeries *series2A = new QtCharts::QAreaSeries(series1, series2);
	//series1A->setName("Sol.1 - Tomato");
	//series2A->setName("Sol.2 - Potato");
	QPen pen(0x059605);
	pen.setWidth(3);
	//series1A->setPen(pen);
	//series2A->setPen(pen);

	QLinearGradient gradient1(QPointF(0, 0), QPointF(0, 1));
	gradient1.setColorAt(0.0, 0x3cc63c);
	gradient1.setColorAt(1.0, 0x26f626);
	gradient1.setCoordinateMode(QGradient::ObjectBoundingMode);
	//series1A->setBrush(gradient1);
	QLinearGradient gradient2(QPointF(0, 0), QPointF(0, 1));
	gradient2.setColorAt(0.0, 0xc63c3c);
	gradient2.setColorAt(1.0, 0xf62626);
	gradient2.setCoordinateMode(QGradient::ObjectBoundingMode);
	//series2A->setBrush(gradient2);
	//![3]
	cout << " set gradients " << endl;


	//![4]
	QtCharts::QChart *chart = new QtCharts::QChart();
	chart->legend()->hide();
	chart->addSeries(m_series_Pon);
	chart->addSeries(m_series_Poff);
	chart->addSeries(m_series_V_switch);
	chart->addSeries(m_series_V_recirc);
	chart->addSeries(m_series_solution);
	chart->addSeries(m_series_ask);
	chart->addSeries(m_series_sync_in);
	chart->addSeries(m_series_sync_out);
	//chart->addSeries(series1A);
	//chart->addSeries(series2A);

	cout << " add series " << endl;


	//  chart->setTitle("Simple areachart example");
	//  chart->createDefaultAxes();
	QtCharts::QCategoryAxis *axisX = new QtCharts::QCategoryAxis();
	QtCharts::QCategoryAxis *axisY = new QtCharts::QCategoryAxis();

	// Customize axis label font
	QFont labelsFont;
	labelsFont.setPixelSize(12);
	axisX->setLabelsFont(labelsFont);
	axisY->setLabelsFont(labelsFont);

	// Customize axis colors
	QPen axisPen(QRgb(0xd18952));
	axisPen.setWidth(2);
	axisX->setLinePen(axisPen);
	axisY->setLinePen(axisPen);

	// Customize axis label colors
	QBrush axisBrush(Qt::black);
	axisX->setLabelsBrush(axisBrush);
	axisY->setLabelsBrush(axisBrush);

	// Customize grid lines and shades
	axisY->setGridLineVisible(false);
	axisY->setShadesPen(Qt::NoPen);
	axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
	axisY->setShadesVisible(true);

	//axisX->append("10", 10);
	axisX->append("20 %", 20);
	//axisX->append("30", 30);
	axisX->append("40 %", 40);
	//axisX->append("50", 50);
	axisX->append("60 %", 60);
	//axisX->append("70", 70);
	axisX->append("80 %", 80);
	//axisX->append("90", 90);
	axisX->append("100 %", 100);
	axisX->setRange(0, 100);

	axisX->setTitleText("Simulation time percentage");

	axisY->append(" ", 10);
	axisY->append("Sync Out", 20);
	axisY->append("Sync In", 30);
	axisY->append("Ask", 40);
	axisY->append("Solution", 50);
	axisY->append("V_recirc", 60);
	axisY->append("V_switch", 70);
	axisY->append("P_off", 80);
	axisY->append("P_on", 90);
	axisY->setRange(0, 100);

	chart->setAxisX(axisX, m_series_X);
	chart->setAxisY(axisY, m_series_Pon);
	chart->setAxisY(axisY, m_series_Poff);
	chart->setAxisY(axisY, m_series_V_switch);
	chart->setAxisY(axisY, m_series_V_recirc);
	chart->setAxisY(axisY, m_series_solution);
	chart->setAxisY(axisY, m_series_ask);
	chart->setAxisY(axisY, m_series_sync_in);
	chart->setAxisY(axisY, m_series_sync_out);
	//chart->createDefaultAxes();

	//chart->axisX()->setRange(0, 20);
	//chart->axisY()->setRange(0, 10);
	//![4]

	//![5]
	QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);
	//![5]

	ui->gridLayout_12->addWidget(chartView);

}

void Labonatip_GUI::setEnableMainWindow(bool _enable) {

	ui->centralwidget->setEnabled(_enable);
	ui->dockWidget->setEnabled(_enable);
	ui->toolBar->setEnabled(_enable);
	ui->toolBar_2->setEnabled(_enable);
	ui->toolBar_3->setEnabled(_enable);

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

}

void Labonatip_GUI::ewst() {

	cout << " whats this mode " << endl;
	QWhatsThis::enterWhatsThisMode();

}

void Labonatip_GUI::about() {

	QMessageBox messageBox;
	messageBox.about(this, tr( "About Fluicell Lab-on-a-tip Wizard"), 
		                   tr( "<b>Lab-on-a-tip</b> is a <a href='http://fluicell.com/'>Fluicell</a> AB software <br>"
							   //"Lab-on-a-tip Wizard <br>"
							   "Copyright Fluicell AB, Sweden 2017 <br> <br>"
							   "Developer: Mauro Bellone http://www.maurobellone.com <br>"
							   "Version 2.0.4" )); // TODO build the string using m_version
	messageBox.setIconPixmap(QPixmap("./icons/fluicell_iconBIG.ico"));
//	messageBox.setFixedSize(500, 700);
}



void Labonatip_GUI::closeEvent(QCloseEvent *event) {

	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Lab-on-a-tip",
		tr("Are you sure?\n"),
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {
		if (m_ppc1->isConnected()) {
			m_ppc1->stop();
			m_ppc1->disconnectCOM(); //if is active, disconnect
		}
		delete m_dialog_tools;
		event->accept();
	}
}

void Labonatip_GUI::setVersion(string _version) {
	m_version = QString::fromStdString(_version);
	this->setWindowTitle(QString("Lab-on-a-tip v.") + m_version);
}

Labonatip_GUI::~Labonatip_GUI ()
{
  delete ui;
  qApp->quit();
}
