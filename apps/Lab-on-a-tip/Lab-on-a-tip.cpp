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
	led_green (new QPixmap( QSize(20, 20))),
	led_red (new QPixmap( QSize(20, 20))),
	m_g_spacer ( new QGroupBox()),
	m_a_spacer (new QAction()),
	m_protocol ( new std::vector<fluicell::PPC1api::command> ),
	m_protocol_duration(0.0),
	m_pen_line_width(7),
	l_x1(-16.0),
	l_y1(49.0),
	l_x2(65.0),
	l_y2(l_y1),
	m_language_idx(0),
	m_base_time_step(1000), //TODO : solve this! there is an issue with the timing of the solution pumped https://stackoverflow.com/questions/21232520/precise-interval-in-qthread
	m_flowing_solution(0),
	m_sol1_color(QColor::fromRgb(255, 189, 0)),//(189, 62, 71)),
	m_sol2_color(QColor::fromRgb(255, 40, 0)),//96, 115, 158)),
	m_sol3_color(QColor::fromRgb(0, 158, 255)),//193, 130, 50)),
	m_sol4_color(QColor::fromRgb(130, 255, 0))//83, 155, 81))
{

  // allows to use path alias
  //QDir::setSearchPaths("icons", QStringList(QDir::currentPath() + "/icons/"));
  
  // setup the user interface
  ui->setupUi (this);

  initCustomStrings();

  ui->dockWidget->close();  //close the advaced dock page
  ui->tabWidget->setCurrentIndex(1);  // put the tab widget to the chart page
  //ui->treeWidget_macroInfo->resizeColumnToContents(0);
  ui->treeWidget_macroInfo->setColumnWidth(0, 200);
  // debug stuff -- set 1 to remove all messages and tab
  if (0)
  {
	  ui->checkBox_dumpToFile->setChecked(false);
	  ui->checkBox_to_terminal->setChecked(false);
	  ui->checkBox_verboseOut->setChecked(false);
	  ui->tabWidget->removeTab(2);
	  setPpc1Verbose(false);
  }
  else {
	  // init the redirect buffer
	  qout = new QDebugStream(std::cout, ui->textEdit_qcout);
	  qout->copyOutToTerminal(ui->checkBox_to_terminal->isChecked());
	  //  QTextStream standardOutput(stdout);
	  qerr = new QDebugStream(std::cerr, ui->textEdit_qcerr);
	  qerr->copyOutToTerminal(ui->checkBox_to_terminal->isChecked());
	  //  QTextStream standardOutput(stderr);// (stdout);
  }

  //TODO: this removes the visualization settings 
  ui->tabWidget->removeTab(3);

  
  // init the object to handle the internal dialogs
  m_dialog_p_editor = new Labonatip_protocol_editor();
  m_dialog_tools = new Labonatip_tools(); 
  
  m_pipette_status = new pipetteStatus();

  m_comSettings = new COMSettings();
  m_solutionParams = new solutionsParams();
  m_pr_params = new pr_params();
  m_GUI_params = new GUIparams();
  *m_comSettings = m_dialog_tools->getComSettings();
  *m_solutionParams = m_dialog_tools->getSolutionsParams();
  *m_pr_params = m_dialog_tools->getPr_params();
  *m_GUI_params = m_dialog_tools->getGUIparams();

  toolRefillSolution();
  toolEmptyWells();

  // set the flows in the table
  ui->treeWidget_macroInfo->topLevelItem(12)->setText(1, QString::number(m_pipette_status->rem_vol_well1));
  ui->treeWidget_macroInfo->topLevelItem(13)->setText(1, QString::number(m_pipette_status->rem_vol_well2));
  ui->treeWidget_macroInfo->topLevelItem(14)->setText(1, QString::number(m_pipette_status->rem_vol_well3));
  ui->treeWidget_macroInfo->topLevelItem(15)->setText(1, QString::number(m_pipette_status->rem_vol_well4));

  // set translation
  QString translation_file = "./languages/eng.qm";
  if (!m_translator.load(translation_file))
	  cout << QDate::currentDate().toString().toStdString() << "  "
	  << QTime::currentTime().toString().toStdString() << "  "
	  << "Labonatip_GUI::Labonatip_GUI ::: translation not loaded" << endl;
  else
	  cout << QDate::currentDate().toString().toStdString() << "  "
	  << QTime::currentTime().toString().toStdString() << "  "
	  << " Translation loaded " << endl;

  qApp->installTranslator(&m_translator);
  switchLanguage(m_dialog_tools->language);

  // all the connects are in this function
  initConnects();
  
  // set the toolbar text icons
  //ui->toolBar_2->setToolButtonStyle(m_GUI_params->showTextToolBar);
  //ui->toolBar_3->setToolButtonStyle(m_GUI_params->showTextToolBar);

  // hide the warning label
  ui->label_warning->hide();
  ui->label_warningIcon->hide();

  // move the arrow in the drawing to point at the solution 1
  ui->widget_solutionArrow->setVisible(false);
  ui->label_arrowSolution->setText(m_solutionParams->sol1);
   
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
  m_ppc1->setCOMport(m_comSettings->getName());
  m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());
  m_ppc1->setVebose(ui->checkBox_verboseOut->isChecked());

  // init thread macroRunner //TODO: this is just a support, check if needed
  m_macroRunner_thread = new Labonatip_macroRunner(this);
  m_macroRunner_thread->setDevice(m_ppc1);

  // status bar to not connected
  led_green->fill(Qt::transparent);
  painter_led_green = new QPainter(led_green);
  QRadialGradient radialGradient_green(8, 8, 12);
  radialGradient_green.setColorAt(0.0, 0xF0F0F0);
  radialGradient_green.setColorAt(0.5, 0x30D030);
  radialGradient_green.setColorAt(1.0, Qt::transparent);
  painter_led_green->setBackground(Qt::blue);
  painter_led_green->setBrush(radialGradient_green);
  painter_led_green->setPen(Qt::gray);
  painter_led_green->drawEllipse(2, 2, 16, 16);

  led_red->fill(Qt::transparent);
  painter_led_red = new QPainter(led_red);
  QRadialGradient radialGradient_red(8, 8, 12);
  radialGradient_red.setColorAt(0.0, 0xF0F0F0);
  radialGradient_red.setColorAt(0.5, 0xFF5050);
  radialGradient_red.setColorAt(1.0, Qt::transparent);
  painter_led_red->setBackground(Qt::blue);
  //painter_led_red->setBrush(Qt::red);
  painter_led_red->setBrush(radialGradient_red);
  painter_led_red->setPen(Qt::gray);
  painter_led_red->drawEllipse(2, 2, 16, 16);

  this->setStatusLed(false);

  ui->status_PPC1_label->setText(m_str_PPC1_status_discon);
  ui->label_macroStatus->setText(m_str_protocol_not_running);

  // init the timers 
  m_update_flowing_sliders = new QTimer();
  m_update_GUI = new QTimer();  
  m_update_waste = new QTimer();
  m_timer_solution = 0;

  m_update_flowing_sliders->setInterval(m_base_time_step);
  m_update_GUI->setInterval(10);// (m_base_time_step);
  m_update_waste->setInterval(m_base_time_step);

  connect(m_update_flowing_sliders, 
	  SIGNAL(timeout()), this, 
	  SLOT(updateTimingSliders()));

  connect(m_update_GUI, 
	  SIGNAL(timeout()), this, 
	  SLOT(updateGUI()));
  m_update_GUI->start();

  connect(m_update_waste,
	  SIGNAL(timeout()), this,
	  SLOT(updateWaste()));
  m_update_waste->start();

  //simulation button activated by default
  ui->actionSimulation->setChecked(true);
  m_simulationOnly = ui->actionSimulation->isChecked();
  ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
  ui->actionReboot->setEnabled(!m_simulationOnly);
  ui->actionShudown->setEnabled(!m_simulationOnly);

  //init the chart view
  m_labonatip_chart_view = new Labonatip_chart();
  m_chartView = m_labonatip_chart_view->getChartView();
  ui->gridLayout_12->addWidget(m_chartView);

  //get the solution colors from the setting file
  QColor c1 = m_solutionParams->sol1_color;
  this->colSolution1Changed(c1.red(), c1.green(), c1.blue());
  QColor c2 = m_solutionParams->sol2_color;
  this->colSolution2Changed(c2.red(), c2.green(), c2.blue());
  QColor c3 = m_solutionParams->sol3_color;
  this->colSolution3Changed(c3.red(), c3.green(), c3.blue());
  QColor c4 = m_solutionParams->sol4_color;
  this->colSolution4Changed(c4.red(), c4.green(), c4.blue());
//  m_labonatip_chart_view->setGUIchart();

  ui->textEdit_emptyTime->setText(" ");


  // set a few shortcuts
  ui->pushButton_solution1->setShortcut(QApplication::translate("Labonatip_GUI", "F1", Q_NULLPTR));
  ui->pushButton_solution2->setShortcut(QApplication::translate("Labonatip_GUI", "F2", Q_NULLPTR));
  ui->pushButton_solution3->setShortcut(QApplication::translate("Labonatip_GUI", "F3", Q_NULLPTR));
  ui->pushButton_solution4->setShortcut(QApplication::translate("Labonatip_GUI", "F4", Q_NULLPTR));

  // instal the event filter on -everything- in the app
  qApp->installEventFilter(this);

  toolApply(); // this is to be sure that the settings are brought into the app at startup

  QString s;
  s.append(m_str_user);
  s.append(" ");
  s.append(m_dialog_tools->getUserName());
  ui->label_user->setText(s);

  cout << QDate::currentDate().toString().toStdString() << "  "
	  << QTime::currentTime().toString().toStdString() << "  "
	  << "Labonatip_GUI::constructor :::: "
	  << m_dialog_tools->getUserName().toStdString() << endl;

}


void Labonatip_GUI::updateMacroStatusMessage(const QString &_message) {
  
	QString s = " PROTOCOL RUNNING : <<<  ";
	s.append(m_dialog_p_editor->getProtocolPath());
	s.append(" >>> remaining time = ");
	
	s.append(_message);
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateMacroStatusMessage :::: " 
		 << _message.toStdString() << endl;
}


void Labonatip_GUI::updateMacroTimeStatus(const double &_status) {

	m_labonatip_chart_view->updateChartTime(_status); // update the vertical line for the time status on the chart

	QString s = m_str_update_time_macro_msg1;
	s.append(m_dialog_p_editor->getProtocolName());
	int remaining_time_sec = m_protocol_duration - _status * m_protocol_duration / 100;
	s.append(m_str_update_time_macro_msg2);
	int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	s.append(" h,   ");
	s.append(QString::number(remaining_mins));
	s.append(" min,   ");
	s.append(QString::number(remaining_secs));
	s.append(" sec   ");
	ui->progressBar_macroStatus->setValue(_status);
	ui->label_macroStatus->setText(s);

	double currentTime = _status * m_protocol_duration / 100.0 ;

	updateFlowControlPercentages();

	if (m_pipette_active) updateDrawing(m_ppc1->getDropletSize());
	else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	//cout << QDate::currentDate().toString().toStdString() << "  " 
	//     << QTime::currentTime().toString().toStdString() << "  "
	//	   << "Labonatip_GUI::updateMacroTimeStatus :::: " << _status << endl;

}

void Labonatip_GUI::askMessage(const QString &_message) {

	QMessageBox::question(this, m_str_ask_msg, _message, m_str_ok);
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

}


void Labonatip_GUI::closeAllValves() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::closeAllValves   " << endl;

	if (m_pipette_active) {
		if (m_ppc1->isConnected()) m_ppc1->closeAllValves();
	}

	//this will stop the solution flow 
	m_timer_solution = std::numeric_limits<int>::max();
	updateDrawing(-1);

}


void Labonatip_GUI::setEnableSolutionButtons(bool _enable ) {
	ui->pushButton_solution1->setEnabled(_enable);
	ui->pushButton_solution2->setEnabled(_enable);
	ui->pushButton_solution3->setEnabled(_enable);
	ui->pushButton_solution4->setEnabled(_enable);
}




void Labonatip_GUI::setAsDefault()
{
	m_pr_params->setDefValues(m_pipette_status->pon_set_point, 
		m_pipette_status->poff_set_point, 
		-m_pipette_status->v_switch_set_point, 
		-m_pipette_status->v_recirc_set_point);

	m_dialog_tools->setDefaultPressuresVacuums(m_pr_params->p_on_default, m_pr_params->p_off_default,
		-m_pr_params->v_recirc_default, -m_pr_params->v_switch_default);

	updateFlowControlPercentages();

	if (m_ppc1)
		m_ppc1->setDefaultPV(m_pr_params->p_on_default, m_pr_params->p_off_default,
			m_pr_params->v_recirc_default, m_pr_params->v_switch_default);

}




void Labonatip_GUI::switchLanguage(int _value )
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::switchLanguage   " << endl;
	

	if (_value == m_language_idx) 
		return; // no translation needed

	// this will re-init custom strings to english for re-translation
	qApp->removeTranslator(&m_translator);
	QString translation_file;
	m_language_idx = _value;
	switch (_value)
	{ 
	case 0:
	{
		translation_file = ":/languages/cn.qm";	
		break;
	}
	case 1:
	{
		translation_file = ":/languages/eng.qm";
		break;
	}
	case 2:
	{
		translation_file = ":/languages/ita.qm";
		break;
	}
	case 3:
	{
		translation_file = ":/languages/sve.qm";
		break;
	}
	default: 
		translation_file = ":/languages/eng.qm"; 
		qApp->removeTranslator(&m_translator);
		break;
	}

	if (m_translator.load(translation_file)) {

		// translate app
		qApp->installTranslator(&m_translator);

		initCustomStrings();
	
		// translate other dialogs
		m_dialog_tools->switchLanguage(translation_file);
		m_dialog_p_editor->switchLanguage(translation_file);



	}
	else cout << " translation not loaded " << endl;

}


void Labonatip_GUI::changeEvent(QEvent* _event)
{
	//cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "Labonatip_GUI::changeEvent   " << _event->type() << endl;

	if (0 != _event) {
		switch (_event->type()) {
			// this event is send if a translator is loaded
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		case QEvent::Resize:
			this->resizeToolbar();
			break;
		case QEvent::WindowStateChange:
			this->resizeToolbar();
			break;
		break;
		}
	}

	QMainWindow::changeEvent(_event);
}

void Labonatip_GUI::resizeEvent(QResizeEvent *_event)
{
	//cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "Labonatip_GUI::resizeEvent   " << _event->type() << endl;
	this->resizeToolbar();
	
}

bool Labonatip_GUI::eventFilter(QObject *_obj, QEvent *_event)
{
	// activate/deactivate tool tips http://www.qtcentre.org/threads/11056-enable-all-Tooltips
	// http://doc.qt.io/qt-4.8/qobject.html#installEventFilter

	if (_event->type() == QEvent::ToolTip) {
		
		if (!m_GUI_params->enableToolTips) { 
			return true; //this filter the event
		}
		else	{		
			// standard event processing
			return QObject::eventFilter(_obj, _event);
		}
	}
	else {
		// standard event processing
		return QObject::eventFilter(_obj, _event);
	}

}

void Labonatip_GUI::setStatusLed( bool _connect ) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::setStatusLed   " << _connect << endl;

	ui->status_PPC1_led->clear();
	if (_connect) {
		ui->status_PPC1_led->setPixmap(*led_green);
	}
	else {
		ui->status_PPC1_led->setPixmap(*led_red);
	}
}

void Labonatip_GUI::initConnects()
{

	//windows toolbar
	connect(ui->actionTools, 
		SIGNAL(triggered()), this, 
		SLOT(showToolsDialog()));
	
	connect(ui->actionEditor,
		SIGNAL(triggered()), this,
		SLOT(showProtocolEditorDialog()));

	connect(ui->actionLoad_profile, 
		SIGNAL(triggered()), this, 
		SLOT(openSettingsFile()));

	connect(ui->actionSave_profile,
		SIGNAL(triggered()), this, 
		SLOT(saveSettingsFile()));

	connect(ui->actionAbout,
		SIGNAL(triggered()), this, 
		SLOT(about()));

	connect(ui->actionAdvanced, 
		SIGNAL(triggered()), this, 
		SLOT(closeOpenDockTools()));

	connect(ui->actionConnectDisconnect, 
		SIGNAL(triggered()), this, 
		SLOT(disCon()));

	connect(ui->actionSimulation,
		SIGNAL(triggered()), this, 
		SLOT(simulationOnly()));

	connect(ui->actionReboot, 
		SIGNAL(triggered()), this, 
		SLOT(reboot()));
	
	connect(ui->actionShudown,
		SIGNAL(triggered()), this, 
		SLOT(shutdown()));

//	connect(ui->actionWhatsthis, 
//		SIGNAL(triggered()), this, 
//		SLOT(ewst()));

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

	connect(ui->pushButton_cleanHistory,
		SIGNAL(clicked()), this,
		SLOT(cleanHistory()));

// this button is connected only if the developer settings tab is visualized
if (ui->tabWidget->count() > 3) 
	connect(ui->pushButton_updateDrawing,
		SIGNAL(clicked()), this,
		SLOT(updateDrawing(100)));

	connect(ui->checkBox_to_terminal, 
		SIGNAL(stateChanged(int)), this, 
		SLOT(dumpToTerminal(int)));

	connect(ui->checkBox_verboseOut, 
		SIGNAL(stateChanged(int)), this, 
		SLOT(setPpc1Verbose(int)));

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
		SIGNAL(emptyWaste()), this,
		SLOT(toolEmptyWells()));

	connect(m_dialog_tools,
		SIGNAL(refillSolution()), this,
		SLOT(toolRefillSolution()));

	connect(m_dialog_tools,
		SIGNAL(ok()), this, 
		SLOT(toolOk()));

	connect(m_dialog_tools, 
		SIGNAL(apply()), this, 
		SLOT(toolApply()));

	connect(m_dialog_p_editor,
		SIGNAL(loadSettingsRequest()), this,
		SLOT(openSettingsFile()));

	connect(m_dialog_p_editor,
		SIGNAL(ok()), this,
		SLOT(editorOk()));

	connect(m_dialog_p_editor,
		SIGNAL(apply()), this,
		SLOT(editorApply()));

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

void Labonatip_GUI::initCustomStrings()
{

	//setting custom strings to translate 
	m_str_areyousure = tr("Are you sure?");
	m_str_waiting = tr("Waiting ...");
	m_str_advanced = tr("Advanced");
	m_str_basic = tr("Basic");
	m_str_operation_cancelled = tr("Operation cancelled");
	m_str_no_file_loaded = tr("No file loaded");
	m_str_information = tr("Information");
	m_str_warning = tr("Warning");
	m_str_error = tr("Error");
	m_str_cancel = tr("Cancel");
	m_str_ok = tr("Ok");
	m_str_PPC1_status_con = tr("PPC1 STATUS: Connected  ");
	m_str_PPC1_status_discon = tr("PPC1 STATUS: NOT Connected  ");
	m_str_protocol_running = tr("Cancel");
	m_str_protocol_not_running = tr("No protocol running  ");
	m_str_connect = tr("Connect");
	m_str_disconnect = tr("Disconnect");
	m_str_save_profile = tr("Save profile");
	m_str_load_profile = tr("Load profile");
	m_str_cannot_save_profile = tr("Cannot save the file");
	m_str_cannot_load_profile = tr("Cannot load the file");
	m_str_warning_simulation_only = tr("The program is in simulation mode");
	m_str_cannot_connect_ppc1 = tr("Unable to connect to PPC1");
	m_str_cannot_connect_ppc1_twice = tr("Unable to connect to PPC1 twice");
	m_str_cannot_connect_ppc1_check_cables = tr("Please, check cables and settings and press Ok");
	m_str_question_find_device = tr("Automatically identify the connected device?");
	m_str_ppc1_connected_but_not_running = tr("PPC1 is connected but not running");
	m_str_question_stop_ppc1 = tr("This will stop the PPC1");
	m_str_unable_stop_ppc1 = tr("Unable to stop and disconnect PPC1");
	m_str_shutdown_pressed = tr("Shutdown pressed, this will take 30 seconds, press Ok to continue, cancel to abort");
	m_str_shutdown_pressed_p_off = tr("The pressure is off, waiting for the vacuum");
	m_str_shutdown_pressed_v_off = tr("Vacuum off, stopping the flow in the device");
	m_str_rebooting = tr("Rebooting ...");
	m_str_reconnecting = tr("Reconnecting ...");
	m_str_initialization = tr("Initialization");
	m_str_newtip_msg1 = tr("Place the pipette into the holder and tighten. THEN PRESS OK");
	m_str_newtip_msg2 = tr("Pressurize the system");
	m_str_newtip_msg3 = tr("Wait until a droplet appears at the tip of the pipette and THEN PRESS OK");
	m_str_newtip_msg4 = tr("Purging the liquid channels");
	m_str_newtip_msg5 = tr("Still purging the liquid channels");
	m_str_newtip_msg6 = tr("Remove the droplet using a lens tissue. THEN PRESS OK");
	m_str_newtip_msg7 = tr("Put the pipette into solution. THEN PRESS OK");
	m_str_newtip_msg8 = tr("Purging the vacuum channels");
	m_str_newtip_msg9 = tr("Establishing operational pressures");
	m_str_newtip_msg10 = tr("Pipette is ready for operation. PRESS OK TO START");
	m_str_standby_operation = tr("Standby operation progress");
	m_str_label_run_protocol = tr("Run protocol");
	m_str_label_stop_protocol = tr("Stop protocol");
	m_str_no_protocol_load_first = tr("No protocol loaded, load one first");
	m_str_loaded_protocol_is = tr("The protocol loaded is : ");
	m_str_protocol_confirm = tr("Press Ok to run the protocol, or press Cancel to load a new one");
	m_str_progress_msg1 = tr("This operation will take ");
	m_str_progress_msg2 = tr(" seconds");
	m_str_ask_msg = tr("Ask message command");
	m_str_editor_apply_msg1 = tr(" No protocol loaded : ");
	m_str_editor_apply_msg2 = tr(" Protocol loaded : ");
	m_str_cleaning_history_msg1 = tr("This will remove all the files in the history folder");
	m_str_cleaning_history_msg2 = tr("History cleaned");
	m_str_update_time_macro_msg1 = tr(" PROTOCOL RUNNING : ");
	m_str_update_time_macro_msg2 = tr(" ----- remaining time,  ");
	m_str_pulse_remaining_time = tr("Pulse time remaining: ");
	m_str_pulse_continuous_flowing = tr("Continuous flowing");
	m_str_pulse_waste = tr("Waste");
	m_str_pulse_full_in = tr("full in");
	m_str_user = tr("User :");

}

void Labonatip_GUI::toolEmptyWells()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::toolEmptyWells   " << endl;

	//TODO: this now is empty and it does not work
	*m_solutionParams = m_dialog_tools->getSolutionsParams();

	m_pipette_status->rem_vol_well5 = 0.0;
	m_pipette_status->rem_vol_well6 = 0.0; 
	m_pipette_status->rem_vol_well7 = 0.0;
	m_pipette_status->rem_vol_well8 = 0.0;

	ui->label_warningIcon->hide();
	ui->label_warning->hide();
	
}

void Labonatip_GUI::toolRefillSolution()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::toolRefillSolution   " << endl;

	//TODO: this now is empty and it does not work
	*m_solutionParams = m_dialog_tools->getSolutionsParams();

	m_pipette_status->rem_vol_well1 = m_solutionParams->vol_well1;
	m_pipette_status->rem_vol_well2 = m_solutionParams->vol_well2;
	m_pipette_status->rem_vol_well3 = m_solutionParams->vol_well3;
	m_pipette_status->rem_vol_well4 = m_solutionParams->vol_well4;

	ui->label_warningIcon->hide();
	ui->label_warning->hide();

	int max_vol_in_well = 30;

	// update wells when the solution is flowing
	{
		m_pipette_status->rem_vol_well1 = m_pipette_status->rem_vol_well1 - //TODO: add check and block for negative values
			0.001 * m_pipette_status->flow_well1;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well1)
			/ max_vol_in_well;
		ui->progressBar_solution1->setValue(int(perc));

		// TODO: there is no check if the remaining solution is zero !

		//waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well1 - // this is in micro liters 10^-6
		//	m_solutionParams->rem_vol_well1) /  //this is in micro liters 10^-6
		//	ui->treeWidget_macroInfo->topLevelItem(4)->text(1).toDouble(); // this is in nano liters 10^-9
	}
	{
		m_pipette_status->rem_vol_well2 = m_pipette_status->rem_vol_well2 -
			0.001 * m_pipette_status->flow_well2;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well2)
			/ max_vol_in_well;
		ui->progressBar_solution2->setValue(int(perc));
	}
	{
		m_pipette_status->rem_vol_well3 = m_pipette_status->rem_vol_well3 -
			0.001 * m_pipette_status->flow_well3;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well3)
			/ max_vol_in_well;
		ui->progressBar_solution3->setValue(int(perc));
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_pipette_status->rem_vol_well4 = m_pipette_status->rem_vol_well4 -
			0.001 * m_pipette_status->flow_well4;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well4)
			/ max_vol_in_well;
		ui->progressBar_solution4->setValue(int(perc));
	}


}


void Labonatip_GUI::toolOk() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::toolOk   " << endl;

	toolApply();
}

void Labonatip_GUI::toolApply()
{

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::toolApply   " << endl;

	*m_comSettings = m_dialog_tools->getComSettings();
	*m_solutionParams = m_dialog_tools->getSolutionsParams();
	*m_pr_params = m_dialog_tools->getPr_params();
	*m_GUI_params = m_dialog_tools->getGUIparams();

	m_dialog_p_editor->setPrParams(*m_pr_params);
	m_dialog_p_editor->setSolParams(*m_solutionParams);

	m_ppc1->setCOMport(m_comSettings->getName());
	m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());

	switchLanguage(m_dialog_tools->language);

	QString s;
	s.append(m_str_user);
	s.append(" ");
	s.append(m_dialog_tools->getUserName());
	ui->label_user->setText(s);

}

void Labonatip_GUI::editorOk()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::editorOk   " << endl;

	editorApply();
}

void Labonatip_GUI::editorApply()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::editorAppy  " << endl;

	if (m_dialog_p_editor->getProtocolPath().isEmpty()) {
		QString s = m_str_editor_apply_msg1;
		ui->label_macroStatus->setText(s);
	}
	else {
		QString s = m_str_editor_apply_msg2;
		s.append(m_dialog_p_editor->getProtocolName());
		ui->label_macroStatus->setText(s);
	}

	m_labonatip_chart_view->updateChartMacro(m_protocol);

	// compute the duration of the macro
	double macro_duration = 0.0;
	for (size_t i = 0; i < m_protocol->size(); i++) {
		if (m_protocol->at(i).getInstruction() ==
			fluicell::PPC1api::command::instructions::sleep)
			macro_duration += m_protocol->at(i).getValue();
	}
	// visualize it in the chart information panel 
	m_protocol_duration = macro_duration;
	QString s = QString::number(m_protocol_duration);
	s.append(" s");
	ui->label_duration->setText(s);

}

void Labonatip_GUI::setEnableMainWindow(bool _enable) {

	ui->centralwidget->setEnabled(_enable);
	ui->dockWidget->setEnabled(_enable);
	ui->toolBar->setEnabled(_enable);
	ui->toolBar_2->setEnabled(_enable);
	ui->toolBar_3->setEnabled(_enable);
}

bool Labonatip_GUI::visualizeProgressMessage(int _seconds, QString _message)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::visualizeProgressMessage   " << _message.toStdString() << endl;

	QString msg = _message;
	msg.append("<br>");
	msg.append(m_str_progress_msg1);
	msg.append(QString::number(_seconds));
	msg.append(m_str_progress_msg2);

	QProgressDialog *PD = new QProgressDialog(msg, m_str_cancel, 0, _seconds, this);
	PD->setMinimumWidth(350);   // here there is a warning that the geometry cannot be set, forget about it!
	PD->setMinimumHeight(150);
	PD->setMaximumWidth(700);
	PD->setMaximumHeight(300);
	PD->setValue(0);
	PD->setMinimumDuration(0); // Change the Minimum Duration before displaying from 4 sec. to 0 sec. 
	PD->show(); // Make sure dialog is displayed immediately
	PD->setValue(1); 
	PD->setWindowModality(Qt::WindowModal);
	//PD->setCancelButtonText(m_str_cancel);// (QApplication::translate("Labonatip_GUI", "Cancel", Q_NULLPTR));

	for (int i = 0; i < _seconds; i++) {
		PD->setValue(i);
		QThread::sleep(1);
		if (PD->wasCanceled()) // the operation cannot be cancelled
		{
			QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
			QMessageBox::information(this, m_str_warning, m_str_operation_cancelled );
			setEnableMainWindow(true);
			return false;
		}
	}
	PD->cancel();
	delete PD;
	return true;

}

void Labonatip_GUI::ewst() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << " whats this mode " << endl;
	QWhatsThis::enterWhatsThisMode();

}

void Labonatip_GUI::cleanHistory()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::cleanHistory   " << endl;


	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_warning,
			QString(m_str_cleaning_history_msg1 + "\n" + m_str_areyousure),
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
	
		QMessageBox::question(this, m_str_information, m_str_operation_cancelled, m_str_ok);
		return;
	}
	else {
		QDir dir(m_ext_data_path);
		dir.setNameFilters(QStringList() << "*.txt");
		dir.setFilter(QDir::Files);
		foreach(QString dirFile, dir.entryList())
		{
			dir.remove(dirFile);
		}
		QMessageBox::question(this, m_str_information, m_str_cleaning_history_msg2, m_str_ok);
		return;
	}

}

void Labonatip_GUI::about() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::about   " << endl;

	QMessageBox messageBox;
	QString msg_title = "About Fluicell Lab-on-a-tip ";
	QString msg_content = tr("<b>Lab-on-a-tip</b> is a <a href='http://fluicell.com/'>Fluicell</a> AB software <br>"
		"Copyright Fluicell AB, Sweden 2017 <br> <br>"
		"Arvid Wallgrens Backe 20<br>"
		"SE-41346 Gothenburg, Sweden<br>"
		"Tel: +46 76 208 3354 <br>"
		"e-mail: info@fluicell.com <br><br>"
		"Developer:<a href='http://www.maurobellone.com'>Mauro Bellone</a> <br>"
		"Version: ");
	msg_content.append(m_version);
	messageBox.about(this, msg_title, msg_content); 
	messageBox.setIconPixmap(QPixmap("./icons/fluicell_iconBIG.ico"));
	messageBox.setFixedSize(600, 800);
}

void Labonatip_GUI::closeEvent(QCloseEvent *event) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::closeEvent   " << endl;


	QMessageBox::StandardButton resBtn = 
		QMessageBox::question(this, m_str_information, m_str_areyousure,
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {

		if (m_macroRunner_thread->isRunning()) this->runMacro(); // this will stop the macro if running

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
		delete m_dialog_p_editor;
		event->accept();
	}
}

void Labonatip_GUI::dumpLogs()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::dumpLogs   " << endl;

	// save log data, messages from the console ect. 
	QString cout_file_name = m_ext_data_path;
	cout_file_name.append("/Cout_");
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
	QString Err_file_name = m_ext_data_path;
	Err_file_name.append("/Err_");
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

	//TODO: add delete class members
  delete m_ppc1;

  delete ui;
  qApp->quit();
}
