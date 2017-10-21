/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
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
	m_macro (NULL),
	m_pen_line_width(7),
	l_x1(-24.0),
	l_y1(49.0),
	l_x2(55.0),
	l_y2(l_y1),
	m_base_time_step(1000), //TODO : solve this! there is an issue with the timing of the solution pumped https://stackoverflow.com/questions/21232520/precise-interval-in-qthread
	m_flowing_solution(0),
	m_pon_set_point(0.0),
	m_poff_set_point(0.0),
	m_v_recirc_set_point(0.0),
	m_v_switch_set_point(0.0),
	m_sol1_color(QColor::fromRgb(255, 189, 0)),//(189, 62, 71)),
	m_sol2_color(QColor::fromRgb(255, 40, 0)),//96, 115, 158)),
	m_sol3_color(QColor::fromRgb(0, 158, 255)),//193, 130, 50)),
	m_sol4_color(QColor::fromRgb(130, 255, 0))//83, 155, 81))
{

  // allows to use path alias
  QDir::setSearchPaths("icons", QStringList(QDir::currentPath() + "/icons/"));
  
  // setup the user interface
  ui->setupUi (this);
  ui->dockWidget->close();  //close the advaced dock page
  ui->treeWidget_macroInfo->setHeaderHidden(false);
  ui->treeWidget_macroInfo->resizeColumnToContents(0);

  // debug stuff -- set 1 to remove all messages and tab
  if (0)
  {
  ui->checkBox_dumpToFile->setChecked(false);
  ui->checkBox_to_terminal->setChecked(false);
  ui->checkBox_verboseOut->setChecked(false);
  ui->tabWidget->removeTab(2);
  setPpc1Verbose(false);
  }

  // set translation
  QString translation_file = "./languages/eng.qm";
  if (!m_translator.load(translation_file)) 
	  cout  << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  " 
			<< "Labonatip_GUI::Labonatip_GUI ::: translation not loaded" << endl;
  else 
	  cout  << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  " 
			<< " Translation loaded " << endl;

  qApp->installTranslator(&m_translator);
  
  // set the stylesheet for the ui-toolbars
  ui->toolBar->setMinimumSize(250, 90);
  ui->toolBar->setStyleSheet("QToolButton:!hover {\n"
	  "background-color:rgb(199, 223, 197)} \n"
	  "QToolBar {background-color:rgb(199, 223, 197)}");
  ui->toolBar_2->setStyleSheet("QToolButton:!hover { \n"
	  "background-color:rgb(199, 223, 197)} \n"
	  "QToolBar {background-color:rgb(199, 223, 197)}");

  // init the object to handle the internal dialogs
  m_dialog_tools = new Labonatip_tools();

  default_pon = m_dialog_tools->m_pr_params->p_on_default;
  default_poff = m_dialog_tools->m_pr_params->p_off_default;
  default_v_switch = -m_dialog_tools->m_pr_params->v_switch_default;
  default_v_recirc = -m_dialog_tools->m_pr_params->v_recirc_default;
  
  switchLanguage(m_dialog_tools->language);

  // all the connects are in this function
  initConnects();
  
  // status bar to not connected
  ui->statusBar->showMessage("STATUS: NOT Connected  ");

  // hide the warning label
  ui->label_warning->hide();
  ui->label_warningIcon->hide();

  // move the arrow in the drawing to point at the solution 1
  ui->widget_solutionArrow->setVisible(false);
  ui->label_arrowSolution->setText(m_dialog_tools->m_solutionParams->sol1);
   
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

  m_pen_line.setColor(Qt::transparent);
  m_pen_line.setWidth(m_pen_line_width);
 
  // initialize PPC1api
  m_ppc1->setCOMport(m_dialog_tools->m_comSettings->name);
  m_ppc1->setBaudRate((int)m_dialog_tools->m_comSettings->baudRate);
  m_ppc1->setVebose(ui->checkBox_verboseOut->isChecked());

  // init the redirect buffer
  qout = new QDebugStream(std::cout, ui->textEdit_qcout);
  qout->copyOutToTerminal(ui->checkBox_to_terminal->isChecked());
  //  QTextStream standardOutput(stdout);
  qerr = new QDebugStream(std::cerr, ui->textEdit_qcerr);
  qerr->copyOutToTerminal(ui->checkBox_to_terminal->isChecked());
  //  QTextStream standardOutput(stderr);// (stdout);

  // init thread macroRunner //TODO: this is just a support, check if needed
  //Labonatip_macroRunner *m_macroRunner_thread = new Labonatip_macroRunner( this );
  m_macroRunner_thread = new Labonatip_macroRunner(this);
  m_macroRunner_thread->setDevice(m_ppc1);

  // init the timers 
  m_update_flowing_sliders = new QTimer();
  m_update_GUI = new QTimer();  
  m_timer_solution = 0;

  m_update_flowing_sliders->setInterval(m_base_time_step);
  m_update_GUI->setInterval(10); //(m_base_time_step);

  connect(m_update_flowing_sliders, 
	  SIGNAL(timeout()), this, 
	  SLOT(updateTimingSliders()));
  connect(m_update_GUI, 
	  SIGNAL(timeout()), this, 
	  SLOT(updateGUI()));
  m_update_GUI->start();

  //simulation button activated by default
  ui->actionSimulation->setChecked(true);
  m_simulationOnly = ui->actionSimulation->isChecked();

  //init the chart view
  m_labonatip_chart_view = new Labonatip_chart();
  m_chartView = m_labonatip_chart_view->getChartView();
  ui->gridLayout_12->addWidget(m_chartView);

  //get the solution colors from the setting file
  QColor c1 = m_dialog_tools->m_solutionParams->sol1_color;
  this->colSolution1Changed(c1.red(), c1.green(), c1.blue());
  QColor c2 = m_dialog_tools->m_solutionParams->sol2_color;
  this->colSolution2Changed(c2.red(), c2.green(), c2.blue());
  QColor c3 = m_dialog_tools->m_solutionParams->sol3_color;
  this->colSolution3Changed(c3.red(), c3.green(), c3.blue());
  QColor c4 = m_dialog_tools->m_solutionParams->sol4_color;
  this->colSolution4Changed(c4.red(), c4.green(), c4.blue());
//  m_labonatip_chart_view->setGUIchart();

  ui->textEdit_emptyTime->setText(" ");

}


void Labonatip_GUI::updateMacroStatusMessage(const QString &_message) {

	QString s = " MACRO RUNNING :: status message >>  ";
	s.append(_message);
	ui->statusBar->showMessage(s);
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateMacroStatusMessage :::: " 
		 << _message.toStdString() << endl;

}


void Labonatip_GUI::updateMacroTimeStatus(const int &_status) {

	QString s = " MACRO RUNNING :: update Macro Time Status >>  ";
	s.append(QString::number(_status));

	m_labonatip_chart_view->updateChartTime(_status); // update the vertical line for the time status on the chart

	double totalTime = ui->treeWidget_macroInfo->topLevelItem(4)->text(1).toDouble();
	double currentTime = _status * totalTime / 100.0 ;

	// visualize it in the chart information panel 
	ui->treeWidget_macroInfo->topLevelItem(5)->setText(1, QString::number(currentTime));

	updateFlowControlPercentages();
	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSize());
	else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	//cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
	//	<< "Labonatip_GUI::updateMacroTimeStatus :::: " << _status << endl;

}

void Labonatip_GUI::askMessage(const QString &_message) {

	QMessageBox::information(this, "Ask message macro command", _message);
	m_macroRunner_thread->askOkEvent(true);

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::askMessage :::: "
		<< _message.toStdString() << endl;

}


void Labonatip_GUI::pumpingOff() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pumpingOff    " << endl;

	if (m_pipette_active) {
		m_ppc1->pumpingOff();
	}

	updatePonSetPoint(0.0);
	updatePoffSetPoint(0.0);
	updateVrecircSetPoint(0.0);
	updateVswitchSetPoint(0.0);

	//this will stop the solution flow 
	m_timer_solution = std::numeric_limits<int>::max();

	// reset the wells
	resetWells();
}


void Labonatip_GUI::closeAllValves() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::closeAllValves   " << endl;

	if (m_pipette_active) {

		m_ppc1->closeAllValves();
	}

	//this will stop the solution flow 
	m_timer_solution = std::numeric_limits<int>::max();

	// reset the wells
	//resetWells();
}


void Labonatip_GUI::setEnableSolutionButtons(bool _enable ) {
	ui->pushButton_solution1->setEnabled(_enable);
	ui->pushButton_solution2->setEnabled(_enable);
	ui->pushButton_solution3->setEnabled(_enable);
	ui->pushButton_solution4->setEnabled(_enable);
}


void Labonatip_GUI::updateDrawing( int _value) {


	if (_value == -1 || _value == 0) { // _value = -1 cleans the scene and make the flow disappear 

		m_scene_solution->clear();
		ui->graphicsView->update();
		ui->graphicsView->show();
		return;
	}

	//clean the scene
	m_scene_solution->clear();

	// draw the circle 
	QBrush brush(m_pen_line.color(), Qt::SolidPattern);

	QPen * border_pen = new QPen();
	border_pen->setColor(Qt::transparent);
	border_pen->setWidth(1);

	// TODO: this is an attempt to make the droplet to look a little bit more realistic
	QPainterPath* path = new QPainterPath();
	path->arcMoveTo(45, 34, 20, 20, -90);  //TODO: a lot of magic numbers !!!! wow !
	path->arcTo(45, 36, 15 + _value / 16.0, 25 +_value / 100.0, -90 - _value / 10.0, 180 + _value / 5.0);
	path->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*path, *border_pen, brush);

	// draw a line from the injector to the solution release point 
	m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);

	ui->graphicsView->setScene(m_scene_solution);
	ui->graphicsView->show();

	return;
}



void Labonatip_GUI::setAsDefault()
{

	default_pon = ui->horizontalSlider_p_on->value();
	default_poff = ui->horizontalSlider_p_off->value();
	default_v_recirc = ui->horizontalSlider_recirculation->value();
	default_v_switch = ui->horizontalSlider_switch->value();

	m_dialog_tools->setDefaultPressuresVacuums( default_pon, default_poff,
			default_v_recirc, default_v_switch);

	updateFlowControlPercentages();

	if (m_ppc1)
		m_ppc1->setDefaultPV(default_pon, default_poff, 
			default_v_recirc, default_v_switch);

}


void Labonatip_GUI::updateFlows()
{
	// calculate the flow
	double delta_pressure = 0.0;
	double outflow = 0.0;
	double inflow_recirculation = 0.0;
	double inflow_switch = 0.0;
	double in_out_ratio = 0.0;
	double solution_usage_off = 0.0;
	double solution_usage_on = 0.0;
	double flow_rate_1 = 0.0;
	double flow_rate_2 = 0.0;
	double flow_rate_3 = 0.0;
	double flow_rate_4 = 0.0;
	double flow_rate_5 = 0.0;
	double flow_rate_6 = 0.0;
	double flow_rate_7 = 0.0;
	double flow_rate_8 = 0.0;
	double v_s = m_v_switch_set_point;
	double p_on = m_pon_set_point;
	double p_off = m_poff_set_point;


	if (!m_simulationOnly) {
		ui->treeWidget_macroInfo->topLevelItem(1)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->outflow, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(2)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->inflow_recirculation, 'g', 2));

		if (m_ppc1->m_PPC1_status->in_out_ratio > 0)
			ui->treeWidget_macroInfo->topLevelItem(3)->setText(1, 
				QString::number(m_ppc1->m_PPC1_status->in_out_ratio, 'g', 2));
		else
			ui->treeWidget_macroInfo->topLevelItem(3)->setText(1, 
				QString::number(0.0, 'g', 2));


		ui->treeWidget_macroInfo->topLevelItem(6)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_1, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(7)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_2, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(8)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_3, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(9)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_4, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(10)->setText(1,
			QString::number(m_ppc1->m_PPC1_status->flow_rate_5, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(11)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_6, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(12)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_7, 'g', 2));
		ui->treeWidget_macroInfo->topLevelItem(13)->setText(1, 
			QString::number(m_ppc1->m_PPC1_status->flow_rate_8, 'g', 2));

		return;
	}
	else{
	// calculate inflow
	delta_pressure = 100.0 * v_s;
	inflow_recirculation = 2.0 * m_ppc1->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

	delta_pressure = 100.0 * (v_s + 2.0 * p_off * (1 - LENGTH_TO_ZONE/LENGTH_TO_TIP)); 
	inflow_switch = 2 * m_ppc1->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

	delta_pressure = 100.0 * 2.0 * p_off;
	solution_usage_off = m_ppc1->getFlowSimple(delta_pressure, 2*LENGTH_TO_ZONE);

	delta_pressure = 100.0 * p_on;
	solution_usage_on = m_ppc1->getFlowSimple(delta_pressure, LENGTH_TO_TIP);


	if (ui->pushButton_solution1->isChecked() ||
		ui->pushButton_solution2->isChecked() ||
		ui->pushButton_solution3->isChecked() ||
		ui->pushButton_solution4->isChecked()) // flow when solution is off // TODO : check on off
		{

		delta_pressure = 100.0 * (p_on + p_off * 3 - v_s * 2.0);

		outflow = m_ppc1->getFlowSimple( delta_pressure, LENGTH_TO_TIP);

		if (ui->pushButton_solution1->isChecked() ) flow_rate_1 = solution_usage_on;
		else flow_rate_1 = solution_usage_off;
		if (ui->pushButton_solution2->isChecked()) flow_rate_2 = solution_usage_on;
		else flow_rate_2 = solution_usage_off;
		if (ui->pushButton_solution3->isChecked()) flow_rate_3 = solution_usage_on;
		else flow_rate_3 = solution_usage_off;
		if (ui->pushButton_solution4->isChecked()) flow_rate_4 = solution_usage_on;
		else flow_rate_4 = solution_usage_off;

	}
	else // flow when solution is on // TODO : check on off
	{
		delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);

		outflow = 2.0 * m_ppc1->getFlowSimple( delta_pressure, 2*LENGTH_TO_ZONE);

		flow_rate_1 = solution_usage_off;
		flow_rate_2 = solution_usage_off;
		flow_rate_3 = solution_usage_off;
		flow_rate_4 = solution_usage_off;

	}

	flow_rate_5 = inflow_switch / 2.0;
	flow_rate_6 = inflow_switch / 2.0;
	flow_rate_7 = inflow_recirculation / 2.0;
	flow_rate_8 = inflow_recirculation / 2.0;

	in_out_ratio = std::abs(outflow / inflow_recirculation);

	ui->treeWidget_macroInfo->topLevelItem(1)->setText(1,
		QString::number(outflow, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(2)->setText(1, 
		QString::number(inflow_recirculation, 'g', 2));
	if (in_out_ratio > 0) ui->treeWidget_macroInfo->topLevelItem(3)->setText(1, 
		QString::number(in_out_ratio, 'g', 2));
	else ui->treeWidget_macroInfo->topLevelItem(3)->setText(1, 
		QString::number(0, 'g', 2));

	ui->treeWidget_macroInfo->topLevelItem(6)->setText(1,
		QString::number(flow_rate_1, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(7)->setText(1, 
		QString::number(flow_rate_2, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(8)->setText(1, 
		QString::number(flow_rate_3, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(9)->setText(1,
		QString::number(flow_rate_4, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(10)->setText(1, 
		QString::number(flow_rate_5, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(11)->setText(1, 
		QString::number(flow_rate_6, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(12)->setText(1,
		QString::number(flow_rate_7, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(13)->setText(1,
		QString::number(flow_rate_8, 'g', 2));
	}

	return;
}


void Labonatip_GUI::updateGUI() {

	if (!m_simulationOnly) {
		int sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_B->sensor_reading);  // rounded to second decimal
		int set_point = (int)(m_ppc1->m_PPC1_data->channel_B->set_point);
		ui->label_switchPressure->setText(QString(QString::number(sensor_reading) + 
			", " + QString::number(set_point) + " mbar"));
		ui->progressBar_switch->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_A->sensor_reading);
		set_point = (int)(m_ppc1->m_PPC1_data->channel_A->set_point);
		ui->label_recircPressure->setText(QString(QString::number(sensor_reading) + 
			", " + QString::number(set_point) + " mbar"));
		ui->progressBar_recirc->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_C->sensor_reading);
		set_point = (int)(m_ppc1->m_PPC1_data->channel_C->set_point);
		ui->label_PoffPressure->setText(QString(QString::number(sensor_reading) + 
			", " + QString::number(set_point) + " mbar"));
		ui->progressBar_pressure_p_off->setValue(sensor_reading);

		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_D->sensor_reading);
		set_point = (int)(m_ppc1->m_PPC1_data->channel_D->set_point);
		ui->label_PonPressure->setText(QString(QString::number(sensor_reading) + 
			", " + QString::number(set_point) + " mbar"));
		ui->progressBar_pressure_p_on->setValue(sensor_reading);

		ui->progressBar_recircIn->setValue(ui->horizontalSlider_recirculation->value());
		ui->progressBar_recircOut->setValue(ui->horizontalSlider_recirculation->value());

		ui->progressBar_switchIn->setValue(ui->horizontalSlider_switch->value());
		ui->progressBar_switchOut->setValue(ui->horizontalSlider_switch->value());

		ui->lcdNumber_dropletSize_percentage->display(m_ppc1->getDropletSize());
		//ui->progressBar_dropletSize->setValue(m_ppc1->getDropletSizePercentage());
		ui->lcdNumber_flowspeed_percentage->display(m_ppc1->getFlowSpeed());
		//ui->progressBar_flowSpeed->setValue(m_ppc1->getFlowSpeedPercentage());
		ui->lcdNumber_vacuum_percentage->display(m_ppc1->getVacuum());
		//ui->progressBar_vacuum->setValue(m_ppc1->getVacuumPercentage());

		//updateDrawing(m_ppc1->getDropletSizePercentage());
	}

	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSize());
	else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());


	updateFlows();


	if (m_ppc1->isRunning())
  	    m_update_GUI->start();
}


void Labonatip_GUI::switchLanguage(int _value )
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::switchLanguage   " << endl;
	
	qApp->removeTranslator(&m_translator);
	QString translation_file;
	switch (_value)
	{ 
	case 0:
	{
		translation_file = "./languages/eng.qm";
		break;
	}
	case 1:
	{
		translation_file = "./languages/sve.qm";
		break;
	}
	case 2:
	{
		translation_file = "./languages/ita.qm";
		break;
	}
	default: 
		translation_file = "./languages/eng.qm";
		qApp->removeTranslator(&m_translator);
		break;
	}

	if (m_translator.load(translation_file)) {
		//m_translator.translate("Labonatip_GUI", "english");
		qApp->installTranslator(&m_translator);

		m_dialog_tools->switchLanguage(translation_file);
	}
	else cout << " translation not loaded " << endl;

}


void Labonatip_GUI::changeEvent(QEvent* _event)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::changeEvent   " << _event->type() << endl;

	if (0 != _event) {
		switch (_event->type()) {
			// this event is send if a translator is loaded
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;


		break;
		}
	}
	QMainWindow::changeEvent(_event);

}


void Labonatip_GUI::initConnects()
{

	//windows toolbar
	connect(ui->actionTools, 
		SIGNAL(triggered()), this, 
		SLOT(showToolsDialog()));

	connect(ui->actionOpen, 
		SIGNAL(triggered()), this, 
		SLOT(openFile()));

	connect(ui->actionSave, 
		SIGNAL(triggered()), this, 
		SLOT(saveFile()));

	connect(ui->actionAbout, 
		SIGNAL(triggered()), this, 
		SLOT(about()));

	connect(ui->actionNerdy, 
		SIGNAL(triggered()), this, 
		SLOT(closeOpenDockTools()));

	connect(ui->actionDisCon, 
		SIGNAL(triggered()), this, 
		SLOT(disCon()));

	connect(ui->actionSimulation, 
		SIGNAL(triggered()), this, 
		SLOT(simulationOnly()));

	//connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(runOperations()));
	connect(ui->actionReset, 
		SIGNAL(triggered()), this, 
		SLOT(reboot()));

	//connect(ui->actionSleep, SIGNAL(triggered()), this, SLOT(pumpingOff()));
	connect(ui->actionShutdown, 
		SIGNAL(triggered()), this, 
		SLOT(shutdown()));

	connect(ui->actionWhatsthis, 
		SIGNAL(triggered()), this, 
		SLOT(ewst()));

	// connect buttons
	connect(ui->pushButton_p_on_down, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePonDown()));

	connect(ui->pushButton_p_on_up, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePonUp()));

	connect(ui->pushButton_p_off_down, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePoffDown()));

	connect(ui->pushButton_p_off_up, 
		SIGNAL(clicked()), this, 
		SLOT(pressurePoffUp()));

	connect(ui->pushButton_switchDown, 
		SIGNAL(clicked()), this, 
		SLOT(pressButtonPressed_switchDown()));

	connect(ui->pushButton_switchUp, 
		SIGNAL(clicked()), this, 
		SLOT(pressButtonPressed_switchUp()));

	connect(ui->pushButton_recirculationDown, 
		SIGNAL(clicked()), this, 
		SLOT(recirculationDown()));

	connect(ui->pushButton_recirculationUp, 
		SIGNAL(clicked()), this, 
		SLOT(recirculationUp()));

	connect(ui->pushButton_solution1, 
		SIGNAL(clicked()), this, 
		SLOT(pushSolution1()));

	connect(ui->pushButton_solution2, 
		SIGNAL(clicked()), this, 
		SLOT(pushSolution2()));

	connect(ui->pushButton_solution3, 
		SIGNAL(clicked()), this, 
		SLOT(pushSolution3()));

	connect(ui->pushButton_solution4, 
		SIGNAL(clicked()), this, 
		SLOT(pushSolution4()));

	connect(ui->pushButton_setValuesAsDefault, 
		SIGNAL(clicked()), this, 
		SLOT(setAsDefault()));


	connect(ui->pushButton_dropSize_minus, 
		SIGNAL(clicked()), this, 
		SLOT(dropletSizeMinus()));

	connect(ui->pushButton_dropSize_plus, 
		SIGNAL(clicked()), this, 
		SLOT(dropletSizePlus()));

	connect(ui->pushButton_flowspeed_minus, 
		SIGNAL(clicked()), this, 
		SLOT(flowSpeedMinus()));

	connect(ui->pushButton_flowspeed_plus,
		SIGNAL(clicked()), this, 
		SLOT(flowSpeedPlus()));

	connect(ui->pushButton_vacuum_minus, 
		SIGNAL(clicked()), this, 
		SLOT(vacuumMinus()));

	connect(ui->pushButton_vacuum_plus, 
		SIGNAL(clicked()), this, 
		SLOT(vacuumPlus()));
	
	connect(ui->pushButton_standby, 
		SIGNAL(clicked()), this, 
		SLOT(standby()));

	connect(ui->pushButton_stop, 
		SIGNAL(clicked()), this, 
		SLOT(stopSolutionFlow()));

	connect(ui->pushButton_operational, 
		SIGNAL(clicked()), this, 
		SLOT(operationalMode()));

	connect(ui->pushButton_runMacro, 
		SIGNAL(clicked()), this, 
		SLOT(runMacro()));

	connect(ui->pushButton_newTip, 
		SIGNAL(clicked()), this, 
		SLOT(newTip()));

	
	connect(ui->checkBox_to_terminal, 
		SIGNAL(stateChanged(int)), this, 
		SLOT(dumpToTerminal(int)));

	connect(ui->checkBox_verboseOut, 
		SIGNAL(stateChanged(int)), this, 
		SLOT(setPpc1Verbose(int)));

	//connect(ui->checkBox_dumpToFile, SIGNAL(stateChanged(int)), this, SLOT(dumpToTerminal(int)));

	// connect sliders
	connect(ui->horizontalSlider_p_on, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderPonChanged(int)));

	connect(ui->horizontalSlider_p_off, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderPoffChanged(int)));

	connect(ui->horizontalSlider_recirculation, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderRecircChanged(int)));

	connect(ui->horizontalSlider_switch, 
		SIGNAL(valueChanged(int)), this, 
		SLOT(sliderSwitchChanged(int)));
	
	connect(m_dialog_tools,
		SIGNAL(emptyWells()), this,
		SLOT(toolEmptyWells()));

	connect(m_dialog_tools,
		SIGNAL(ok()), this, 
		SLOT(toolOk()));

	connect(m_dialog_tools, 
		SIGNAL(apply()), this, 
		SLOT(toolApply()));

	connect(m_dialog_tools,
		&Labonatip_tools::colSol1Changed, this,
		&Labonatip_GUI::colSolution1Changed);

	connect(m_dialog_tools,
		&Labonatip_tools::colSol2Changed, this,
		&Labonatip_GUI::colSolution2Changed);

	connect(m_dialog_tools,
		&Labonatip_tools::colSol3Changed, this,
		&Labonatip_GUI::colSolution3Changed);

	connect(m_dialog_tools,
		&Labonatip_tools::colSol4Changed, this,
		&Labonatip_GUI::colSolution4Changed);
}

void Labonatip_GUI::toolEmptyWells()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::toolEmptyWells   " << endl;

	ui->progressBar_recircIn->setValue(0);
	ui->progressBar_recircOut->setValue(0);
	ui->progressBar_switchIn->setValue(0); 
	ui->progressBar_switchOut->setValue(0);

	
}

void Labonatip_GUI::toolOk() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::toolOk   " << endl;

	m_ppc1->setCOMport(m_dialog_tools->m_comSettings->name);
	m_ppc1->setBaudRate((int)m_dialog_tools->m_comSettings->baudRate);

	switchLanguage(m_dialog_tools->language);

	/////////////////////////////////////////
	// TODO set all the other options
	if (m_dialog_tools->isContinuousFlowing())
	{
		m_update_flowing_sliders->start();
		QString s;
		s.append(" Continuous \n flowing");
		ui->textEdit_emptyTime->setText(s);
	}
	else
	{
		QString s;
		s.append("Well empty in \n");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		int remaining_secs = remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, \n");
		s.append(QString::number(remaining_mins));
		s.append(" min \n");
		s.append(QString::number(remaining_secs));
		s.append(" sec ");
	}

	//updateChartMacro();
	m_labonatip_chart_view->updateChartMacro(m_macro);

	// compute the duration of the macro
	double macro_duration = 0.0;
	for (size_t i = 0; i < m_macro->size(); i++) {
		if (m_macro->at(i).getInstruction() == 
			fluicell::PPC1api::command::instructions::sleep)
			macro_duration += m_macro->at(i).getValue();
	}
	// visualize it in the chart information panel 
	ui->treeWidget_macroInfo->topLevelItem(4)->setText(1, QString::number(macro_duration));
}

void Labonatip_GUI::toolApply()
{

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::toolApply   " << endl;

	m_ppc1->setCOMport(m_dialog_tools->m_comSettings->name);
	m_ppc1->setBaudRate((int)m_dialog_tools->m_comSettings->baudRate);


	/////////////////////////////////////////
	// TODO set all the other options
	if (m_dialog_tools->isContinuousFlowing())
	{
		m_update_flowing_sliders->start();
		QString s;
		s.append(" Continuous \n flowing");
		ui->textEdit_emptyTime->setText(s);
	}
	else
	{
		QString s;
		s.append("Well empty in \n");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		int remaining_secs = remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, \n");
		s.append(QString::number(remaining_mins));
		s.append(" min \n");
		s.append(QString::number(remaining_secs));
		s.append(" sec ");
	}

	m_labonatip_chart_view->updateChartMacro(m_macro);

	// compute the duration of the macro
	double macro_duration = 0.0;
	for (size_t i = 0; i < m_macro->size(); i++) {
		if (m_macro->at(i).getInstruction() ==
			fluicell::PPC1api::command::instructions::sleep)
			macro_duration += m_macro->at(i).getValue();
	}
	// visualize it in the chart information panel 
	ui->treeWidget_macroInfo->topLevelItem(4)->setText(1, QString::number(macro_duration));
}


void Labonatip_GUI::setEnableMainWindow(bool _enable) {

	ui->centralwidget->setEnabled(_enable);
	ui->dockWidget->setEnabled(_enable);
	ui->toolBar->setEnabled(_enable);
	ui->toolBar_2->setEnabled(_enable);

}



bool Labonatip_GUI::visualizeProgressMessage(int _seconds, QString _message)
{
	QString msg = _message;
	msg.append("<br>This operation will take ");
	msg.append(QString::number(_seconds));
	msg.append(" seconds.");

	QProgressDialog *PD = new QProgressDialog(msg, "Cancel", 0, _seconds, this);
	//QFont font;
	//font.setPixelSize(16);
	//PD->setFont(font);  // TODO uniform all the font in the application
	PD->setMinimumWidth(350);
	PD->setMinimumHeight(150);
	PD->setMaximumWidth(700);
	PD->setMaximumHeight(300);
	PD->setValue(0);
	PD->setMinimumDuration(0); // Change the Minimum Duration before displaying from 4 sec. to 0 sec. 
	PD->show(); // Make sure dialog is displayed immediately
	PD->setValue(1); 
	PD->setWindowModality(Qt::WindowModal);
	for (int i = 0; i < _seconds; i++) {
		PD->setValue(i);
		QThread::sleep(1);
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, "Warning !", " Operation cancelled  ");
			setEnableMainWindow(true);
			return false;
		}
	}
	PD->cancel();
	return true;

}


void Labonatip_GUI::ewst() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << " whats this mode " << endl;
	QWhatsThis::enterWhatsThisMode();

}

void Labonatip_GUI::about() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::about  " << endl;

	QMessageBox messageBox;
	QString msg_title = "About Fluicell Lab-on-a-tip Wizard";
	QString msg_content = tr("<b>Lab-on-a-tip</b> is a <a href='http://fluicell.com/'>Fluicell</a> AB software <br>"
		//"Lab-on-a-tip Wizard <br>"
		"Copyright Fluicell AB, Sweden 2017 <br> <br>"
		"Developer: Mauro Bellone <a href='http://www.maurobellone.com'>http://www.maurobellone.com</a> <br>"
		"Version: ");
	msg_content.append(m_version);
	messageBox.about(this, msg_title, msg_content); 
	messageBox.setIconPixmap(QPixmap("./icons/fluicell_iconBIG.ico"));
	messageBox.setFixedSize(500, 700);
}



void Labonatip_GUI::closeEvent(QCloseEvent *event) {

	QMessageBox::StandardButton resBtn = 
		QMessageBox::question(this, "Lab-on-a-tip",
		tr("Are you sure?\n"),
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {

		// dump log file
		if (ui->checkBox_dumpToFile->isChecked())
		{
			// save log data, messages from the console ect. 
			dumpLogs();
		}
		if (m_ppc1->isConnected()) {
			
			m_ppc1->stop();
			m_ppc1->disconnectCOM(); //if is active, disconnect
		}
		delete m_dialog_tools;
		event->accept();
	}
}

void Labonatip_GUI::dumpLogs()
{
	// save log data, messages from the console ect. 
	QString cout_file_name = QString("./Ext_data/Cout_");
	cout_file_name.append(QDate::currentDate().toString());
	cout_file_name.append("_");
	cout_file_name.append(QString::number(QTime::currentTime().hour()));
	cout_file_name.append("_");
	cout_file_name.append(QString::number(QTime::currentTime().minute()));
	cout_file_name.append("_");
	cout_file_name.append(QString::number(QTime::currentTime().second()));
	cout_file_name.append(".txt");

	QFile coutfile;
	coutfile.setFileName(cout_file_name);
	coutfile.open(QIODevice::Append | QIODevice::Text);
	QTextStream c_out(&coutfile);
	c_out << ui->textEdit_qcout->toPlainText() << endl;

	// save log data, messages from the console ect. 
	QString Err_file_name = QString("./Ext_data/Err_");
	Err_file_name.append(QDate::currentDate().toString());
	Err_file_name.append("_");
	Err_file_name.append(QString::number(QTime::currentTime().hour()));
	Err_file_name.append("_");
	Err_file_name.append(QString::number(QTime::currentTime().minute()));
	Err_file_name.append("_");
	Err_file_name.append(QString::number(QTime::currentTime().second()));
	Err_file_name.append(".txt");

	QFile cerrfile;
	cerrfile.setFileName(Err_file_name);
	cerrfile.open(QIODevice::Append | QIODevice::Text);
	QTextStream c_err(&cerrfile);
	c_err << ui->textEdit_qcerr->toPlainText() << endl;

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
