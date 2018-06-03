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
	m_cmd_idx_c(0), m_cmd_command_c(1), m_cmd_range_c(2),
	m_cmd_value_c(3), m_cmd_msg_c(4), m_cmd_level_c(5),
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

  // initialize the tools as we need the settings
  m_dialog_tools = new Labonatip_tools();
  m_dialog_tools->setExtDataPath(m_ext_data_path);
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

  // init the object to handle the internal dialogs
  //m_dialog_p_editor = new Labonatip_protocol_editor();
  //m_dialog_p_editor->setVersion(m_version);

  ui->dockWidget->close();  //close the advanced dock page
  ui->tabWidget->setCurrentIndex(1);  // put the tab widget to the chart page
  //ui->treeWidget_macroInfo->resizeColumnToContents(0);
  ui->treeWidget_macroInfo->setColumnWidth(0, 200);

  // debug stuff -- set 1 to remove all messages and tab
  if (0)
  {
	  ui->tabWidget->removeTab(2);
  }
  else {
	  // init the redirect buffer
	  qout = new QDebugStream(std::cout, ui->textEdit_qcout);
#ifndef _DEBUG
	  qout->copyOutToTerminal(m_GUI_params->enableHistory);
#else
	  qout->copyOutToTerminal(false);
#endif
	  qout->redirectOutInGUI(m_GUI_params->enableHistory);
	  qerr = new QDebugStream(std::cerr, ui->textEdit_qcerr);
	  qerr->copyOutToTerminal(m_GUI_params->enableHistory);
	  qerr->redirectOutInGUI(m_GUI_params->enableHistory);
  }

  // this removes the visualization settings 
  ui->tabWidget->removeTab(3);
  ui->stackedWidget_main->setCurrentIndex(0);
  ui->stackedWidget_indock->setCurrentIndex(0);
  //ui->dockWidget->setMinimumWidth(180);

  m_reader = new Labonatip_protocolReader(ui->treeWidget_macroTable);
  m_writer = new Labonatip_protocolWriter(ui->treeWidget_macroTable);

  // set the flows in the table
  ui->treeWidget_macroInfo->topLevelItem(12)->setText(1,
	  QString::number(m_pipette_status->rem_vol_well1));
  ui->treeWidget_macroInfo->topLevelItem(13)->setText(1,
	  QString::number(m_pipette_status->rem_vol_well2));
  ui->treeWidget_macroInfo->topLevelItem(14)->setText(1, 
	  QString::number(m_pipette_status->rem_vol_well3));
  ui->treeWidget_macroInfo->topLevelItem(15)->setText(1, 
	  QString::number(m_pipette_status->rem_vol_well4));

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
	  // set the scene rectangle to avoid the graphic area to move
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
  m_ppc1->setVerbose(m_pr_params->enableFilter);
  m_ppc1->setFilterSize(m_pr_params->filterSize);

  // init thread macroRunner 
  m_macroRunner_thread = new Labonatip_macroRunner(this);
  m_macroRunner_thread->setDevice(m_ppc1);

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
  this->switchLanguage(m_GUI_params->language);

  //speech synthesis
  m_speech = new QTextToSpeech(this);
  

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

  ui->treeWidget_params->resizeColumnToContents(0);
  ui->treeWidget_params->setHeaderHidden(false);

  // reset the macrotable widget
  ui->treeWidget_macroTable->setColumnWidth(m_cmd_idx_c, 70);
  ui->treeWidget_macroTable->setColumnWidth(m_cmd_command_c, 240);
  ui->treeWidget_macroTable->setColumnWidth(m_cmd_range_c, 160);
  ui->treeWidget_macroTable->setColumnWidth(m_cmd_value_c, 100);

  // set delegates
  m_combo_delegate = new ComboBoxDelegate();
  m_no_edit_delegate = new NoEditDelegate();
  m_no_edit_delegate2 = new NoEditDelegate();
  m_spinbox_delegate = new SpinBoxDelegate();
  ui->treeWidget_macroTable->setItemDelegateForColumn(0, new NoEditDelegate(this));
  ui->treeWidget_macroTable->setItemDelegateForColumn(1, new ComboBoxDelegate(this));
  ui->treeWidget_macroTable->setItemDelegateForColumn(2, new NoEditDelegate(this));
  ui->treeWidget_macroTable->setItemDelegateForColumn(3, new SpinBoxDelegate(this));

  ui->treeWidget_macroTable->setContextMenuPolicy(
	  Qt::CustomContextMenu);
  ui->treeWidget_protocol_folder->setContextMenuPolicy(
	  Qt::CustomContextMenu);

  // the undo
  m_undo_stack = new QUndoStack(this);

  m_undo_view = new QUndoView(m_undo_stack);
  m_undo_view->setWindowTitle(tr("Command List"));
  m_undo_view->window()->setMinimumSize(300, 300);
  m_undo_view->setAttribute(Qt::WA_QuitOnClose, false);
  ui->pushButton_undo->setShortcut(
	  QApplication::translate("Labonatip_protocol_editor",
		  "Ctrl+Z", Q_NULLPTR));

  ui->pushButton_redo->setShortcut(
	  QApplication::translate("Labonatip_protocol_editor",
		  "Ctrl+Y", Q_NULLPTR));

  ui->pushButton_removeMacroCommand->setShortcut(
	  QApplication::translate("Labonatip_protocol_editor",
		  "Del", Q_NULLPTR));

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

  //simulation button not-activated by default
  ui->actionSimulation->setChecked(false);
  m_simulationOnly = ui->actionSimulation->isChecked();
  ui->actionConnectDisconnect->setEnabled(!m_simulationOnly);
  ui->actionReboot->setEnabled(!m_simulationOnly);
  ui->actionShudown->setEnabled(!m_simulationOnly);
  ui->actionReboot->setEnabled(false);
  ui->actionShudown->setEnabled(false);
  ui->groupBox_action->setEnabled(false);
  ui->groupBox_deliveryZone->setEnabled(false);
  ui->groupBox_3->setEnabled(false);
  ui->tab_2->setEnabled(false);

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
  ui->pushButton_solution1->setShortcut(
	  QApplication::translate("Labonatip_GUI", "F1", Q_NULLPTR));
  ui->pushButton_solution2->setShortcut(
	  QApplication::translate("Labonatip_GUI", "F2", Q_NULLPTR));
  ui->pushButton_solution3->setShortcut(
	  QApplication::translate("Labonatip_GUI", "F3", Q_NULLPTR));
  ui->pushButton_solution4->setShortcut(
	  QApplication::translate("Labonatip_GUI", "F4", Q_NULLPTR));

  // install the event filter on -everything- in the app
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



void Labonatip_GUI::askMessage(const QString &_message) {

	if (m_GUI_params->speechActive)  m_speech->say(_message);
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
	
		// translate other dialogs and objects
		m_dialog_tools->switchLanguage(translation_file);
		m_macroRunner_thread->switchLanguage(translation_file);
		m_reader->switchLanguage(translation_file);
		m_writer->switchLanguage(translation_file);

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
		SLOT(loadPressed()));

	connect(ui->actionSave_profile,
		SIGNAL(triggered()), this, 
		SLOT(savePressed()));

	connect(ui->actionAbout,
		SIGNAL(triggered()), this, 
		SLOT(about()));

	connect(ui->actionAdvanced, 
		SIGNAL(triggered()), this, 
		SLOT(closeOpenDockTools()));

	connect(ui->actionConnectDisconnect, 
		SIGNAL(triggered(bool)), this,
		SLOT(disCon(bool)));

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

	connect(ui->pushButton_set_preset1,
		SIGNAL(clicked()), this,
		SLOT(setPreset1()));

	connect(ui->pushButton_set_preset2,
		SIGNAL(clicked()), this,
		SLOT(setPreset2()));

	connect(ui->pushButton_set_preset3,
		SIGNAL(clicked()), this,
		SLOT(setPreset3()));

	connect(ui->pushButton_reset_preset1,
		SIGNAL(clicked()), this,
		SLOT(resetPreset1()));

	connect(ui->pushButton_reset_preset2,
		SIGNAL(clicked()), this,
		SLOT(resetPreset2()));

	connect(ui->pushButton_reset_preset3,
		SIGNAL(clicked()), this,
		SLOT(resetPreset3()));

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
		SLOT(stopFlow()));

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

	//connect(m_dialog_p_editor,
	//	SIGNAL(loadSettingsRequest()), this,
	//	SLOT(openSettingsFile()));

	//connect(m_dialog_p_editor,
	//	SIGNAL(ok()), this,
	//	SLOT(editorOk()));

	//connect(m_dialog_p_editor,
	//	SIGNAL(apply()), this,
	//	SLOT(editorApply()));

	connect(m_dialog_tools,
		&Labonatip_tools::TTLsignal, this,
		&Labonatip_GUI::testTTL);

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

	connect(ui->treeWidget_protocol_folder,
		SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onProtocolClicked(QTreeWidgetItem*, int)));

	connect(ui->pushButton_openFolder,
		SIGNAL(clicked()), this, SLOT(openProtocolFolder())); 

	connect(ui->treeWidget_protocol_folder,
		SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(protocolsMenu(const QPoint&)));

	connect(ui->pushButton_undo,
		SIGNAL(clicked()), this, SLOT(undo()));

	connect(ui->pushButton_redo,
		SIGNAL(clicked()), this, SLOT(redo()));

	connect(ui->treeWidget_macroTable,
		SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
		SLOT(itemChanged(QTreeWidgetItem *, int)));

	connect(ui->pushButton_addMacroCommand,
		SIGNAL(clicked()), this, SLOT(addCommand()));


	connect(ui->pushButton_removeMacroCommand,
		SIGNAL(clicked()), this, SLOT(removeCommand()));

	connect(ui->pushButton_moveDown,
		SIGNAL(clicked()), this, SLOT(moveDown()));

	connect(ui->pushButton_moveUp,
		SIGNAL(clicked()), this, SLOT(moveUp()));

	connect(ui->pushButton_plusIndent,
		SIGNAL(clicked()), this, SLOT(plusIndent()));

	connect(ui->pushButton_duplicateLine,
		SIGNAL(clicked()), this, SLOT(duplicateItem()));

	connect(ui->pushButton_clearCommands,
		SIGNAL(clicked()), this, SLOT(clearAllCommandsRequest()));

	connect(ui->pushButton_loop,
		SIGNAL(clicked()), this, SLOT(createNewLoop()));


}

void Labonatip_GUI::testTTL(bool _state) {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::testTTL   " << _state << endl;

	if (m_ppc1->isConnected())
	{
		m_ppc1->setTTLstate(_state);
	}
	else
	{
		QMessageBox::warning(this, m_str_warning,
			QString("PPC1 not connected, TTL cannot run"));
	}
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
	m_str_save = tr("Save");
	m_str_save_profile = tr("Save profile");
	m_str_load = tr("Load");
	m_str_load_profile = tr("Load profile");
	m_str_commander = tr("Commander");
	m_str_editor = tr("Editor");
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
	m_str_stop_1 = tr("Pumps are off, depressuring ...");
	m_str_stop_2 = tr("Depressuring ...");
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
	m_str_operation_cannot_be_done = tr("Operation cannot be done");
	m_str_out_of_bound = tr("Please, check for out of bound values");
	m_str_user = tr("User :");
	m_str_protocol_running_stop = tr("A protocol is running, stop the protocol first");
	m_str_lost_connection = tr("Lost connection with PPC1");
	m_str_swapping_to_simulation = tr("swapping to simulation mode");
	m_str_warning_solution_end = tr("Warning: the solution is running out");
	m_str_warning_waste_full = tr("Warning: the waste is full");
	m_str_save_protocol = tr("Save profile");
	//m_str_load_protocol = tr("Load profile");
	m_str_select_folder = tr("Select folder");
	//m_str_file_not_found = tr("File not found");
	m_str_file_not_saved = tr("File not saved");
	m_str_protocol_duration = tr("Protocol duration : ");
	//m_str_check_validity_protocol = tr("Check validity failed during macro saving");
	//m_str_check_validity_protocol_try_again = tr("Please check your settings and try again");
	//m_str_negative_level = tr("Negative level, file corrupted");
	m_str_remove_file = tr("This action will remove the file, are you sure?");
	m_str_current_prot_name = tr("The current protocol file name is");
	m_str_question_override = tr("Do you want to override?");
	m_str_override_guide = tr(" Yes = override, NO = saveAs, Cancel = do nothing");
	m_str_add_protocol_bottom = tr("Do you want to add to the bottom of the protocol?");
	m_str_add_protocol_bottom_guide = tr("Click NO to clean the workspace and load a new protocol");
	m_str_clear_commands = tr("This will clear all items in the current protocol");
}

void Labonatip_GUI::appScaling(int _dpiX, int _dpiY)
{

	

	QSize toolbar_icon_size = ui->toolBar->iconSize();
	toolbar_icon_size.scale(toolbar_icon_size*_dpiX/100, Qt::KeepAspectRatioByExpanding);
	ui->toolBar->setIconSize(toolbar_icon_size);

	toolbar_icon_size = ui->toolBar_2->iconSize();
	toolbar_icon_size.scale(toolbar_icon_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->toolBar_2->setIconSize(toolbar_icon_size);

	toolbar_icon_size = ui->toolBar_3->iconSize();
	toolbar_icon_size.scale(toolbar_icon_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->toolBar_3->setIconSize(toolbar_icon_size);

	QSize gr_b_action = ui->groupBox_action->minimumSize();
	gr_b_action.scale(gr_b_action*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->groupBox_action->setMinimumSize(gr_b_action);

	QSize gr_delivery_min = ui->groupBox_deliveryZone->minimumSize();
	gr_delivery_min.scale(gr_delivery_min*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->groupBox_deliveryZone->setMinimumSize(gr_delivery_min);

	QSize gr_delivery_max = ui->groupBox_deliveryZone->maximumSize();
	gr_delivery_max.scale(gr_delivery_max*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->groupBox_deliveryZone->setMaximumSize(gr_delivery_max);

	QSize gr_5 = ui->groupBox_5->minimumSize();
	gr_5.scale(gr_5*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->groupBox_5->setMinimumSize(gr_5);

	//QSize gr_chart = ui->groupBox_chart->minimumSize();
	//gr_chart.scale(gr_chart*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	//ui->groupBox_chart->setMinimumSize(gr_chart);

	QSize action_button_size = ui->pushButton_newTip->minimumSize();
	action_button_size.scale(action_button_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->pushButton_newTip->setMinimumSize(action_button_size);
	ui->pushButton_runMacro->setMinimumSize(action_button_size);
	ui->pushButton_operational->setMinimumSize(action_button_size);
	ui->pushButton_stop->setMinimumSize(action_button_size);
	ui->pushButton_standby->setMinimumSize(action_button_size);

	QSize action_button_icon_size = ui->pushButton_newTip->iconSize();
	action_button_icon_size.scale(action_button_icon_size*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->pushButton_newTip->setIconSize(action_button_icon_size);
	ui->pushButton_runMacro->setIconSize(action_button_icon_size);
	ui->pushButton_operational->setIconSize(action_button_icon_size);
	ui->pushButton_stop->setIconSize(action_button_icon_size);
	ui->pushButton_standby->setIconSize(action_button_icon_size);

	QSize delivery_zone_buttons = ui->pushButton_dropSize_minus->size();
	delivery_zone_buttons.scale(delivery_zone_buttons*_dpiX / 100, Qt::KeepAspectRatioByExpanding);
	ui->pushButton_dropSize_minus->setFixedWidth(delivery_zone_buttons.height());
	ui->pushButton_dropSize_plus->setFixedWidth(delivery_zone_buttons.height());
	ui->pushButton_flowspeed_minus->setFixedWidth(delivery_zone_buttons.height());
	ui->pushButton_flowspeed_plus->setFixedWidth(delivery_zone_buttons.height());
	ui->pushButton_vacuum_minus->setFixedWidth(delivery_zone_buttons.height());
	ui->pushButton_vacuum_plus->setFixedWidth(delivery_zone_buttons.height());
	ui->pushButton_dropSize_minus->setFixedHeight(delivery_zone_buttons.height());
	ui->pushButton_dropSize_plus->setFixedHeight(delivery_zone_buttons.height());
	ui->pushButton_flowspeed_minus->setFixedHeight(delivery_zone_buttons.height());
	ui->pushButton_flowspeed_plus->setFixedHeight(delivery_zone_buttons.height());
	ui->pushButton_vacuum_minus->setFixedHeight(delivery_zone_buttons.height());
	ui->pushButton_vacuum_plus->setFixedHeight(delivery_zone_buttons.height());


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

	//m_dialog_p_editor->setPrParams(*m_pr_params);
	//m_dialog_p_editor->setSolParams(*m_solutionParams);
	ui->treeWidget_params->topLevelItem(0)->setText(1, m_solutionParams->sol1);
	ui->treeWidget_params->topLevelItem(1)->setText(1, m_solutionParams->sol2);
	ui->treeWidget_params->topLevelItem(2)->setText(1, m_solutionParams->sol3);
	ui->treeWidget_params->topLevelItem(3)->setText(1, m_solutionParams->sol4);

	ui->treeWidget_params->topLevelItem(4)->setText(1, QString::number(m_pr_params->p_on_default));
	ui->treeWidget_params->topLevelItem(5)->setText(1, QString::number(m_pr_params->p_off_default));
	ui->treeWidget_params->topLevelItem(6)->setText(1, QString::number(m_pr_params->v_recirc_default));
	ui->treeWidget_params->topLevelItem(7)->setText(1, QString::number(m_pr_params->v_switch_default));
	//m_protocolWizard->setSolParams(*m_solutionParams);
	//m_protocolWizard->setPrParams(*m_pr_params);


	m_ppc1->setCOMport(m_comSettings->getName());
	m_ppc1->setBaudRate((int)m_comSettings->getBaudRate());
	m_ppc1->setFilterEnabled(m_pr_params->enableFilter);
	m_ppc1->setFilterSize(m_pr_params->filterSize);
	m_ppc1->setVerbose(m_pr_params->verboseOut);
	m_ext_data_path = m_GUI_params->outFilePath;
	qout->copyOutToTerminal(m_GUI_params->enableHistory);
	qerr->copyOutToTerminal(m_GUI_params->enableHistory);
	qout->redirectOutInGUI(m_GUI_params->enableHistory);
	qerr->redirectOutInGUI(m_GUI_params->enableHistory);

	this->switchLanguage(m_GUI_params->language);

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

	//if (m_dialog_p_editor->getProtocolPath().isEmpty()) {
	//	QString s = m_str_editor_apply_msg1;
	//	ui->label_macroStatus->setText(s); //TODO
	//}
	//else {
	//	QString s = m_str_editor_apply_msg2;
	//	s.append(m_dialog_p_editor->getProtocolName());
	//	ui->label_macroStatus->setText(s);
	//}

	//*m_protocol = m_dialog_p_editor->getProtocol();
	m_labonatip_chart_view->updateChartProtocol(m_protocol);

	// compute the duration of the macro
	double macro_duration = 0.0;
	for (size_t i = 0; i < m_protocol->size(); i++) {
		if (m_protocol->at(i).getInstruction() ==
			fluicell::PPC1api::command::instructions::wait)
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

	//if (m_GUI_params->speechActive)  m_speech->say(_message);

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
	messageBox.setIconPixmap(QPixmap(":/icons/fluicell_iconBIG.ico"));
	messageBox.setFixedSize(600, 800);
}

void Labonatip_GUI::readProtocolFolder(QString _path)
{
	ui->treeWidget_protocol_folder->clear();

	ui->lineEdit_protocolPath->setText(_path);

	QStringList filters;
	filters << "*.prt";

	QDir protocol_path;
	protocol_path.setPath(_path);
	QStringList list = protocol_path.entryList(filters);

	for (int i = 0; i < list.size(); i++) // starting from 2 it will not add ./ and ../
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, list.at(i));
		ui->treeWidget_protocol_folder->addTopLevelItem(item);
		//TODO: here there is a possible memory leak, the pointer *item will never be removed anymore outside the cycle
		//      however, ->addTopLevelItem only accepts pointers 
		//      this means that if one dereferentiate *item that will be removed automatically from the list
	}

}

void Labonatip_GUI::openProtocolFolder()
{
	QDir path = QFileDialog::getExistingDirectory(this, m_str_select_folder, m_protocol_path);

	QString pp = path.path();
	if (pp != ".") // this prevent cancel to delete the old path
		setProtocolUserPath(pp);
}

void Labonatip_GUI::onProtocolClicked(QTreeWidgetItem *item, int column)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_GUI::onProtocolClicked " << endl;

	QString file = item->text(0);
	m_current_protocol_file_name = file;
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append(file);

	// TODO: the wait cursor does not work if called after the message !
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode


	QMessageBox::StandardButton resBtn = QMessageBox::question(this, m_str_warning,
		m_str_add_protocol_bottom + "<br>" + m_str_add_protocol_bottom_guide,
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);


	if (resBtn == QMessageBox::Yes) {
		m_reader->readProtocol(protocol_path);
		addAllCommandsToProtocol();
		m_current_protocol_file_path = protocol_path;
	}
	if (resBtn == QMessageBox::No)
	{
		clearAllCommands(); //TODO
		m_reader->readProtocol(protocol_path);
		addAllCommandsToProtocol();
		m_current_protocol_file_path = protocol_path;

	}
	if (resBtn == QMessageBox::Cancel)
	{
		//do nothing
	}
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}

void Labonatip_GUI::addAllCommandsToProtocol()
{
	/////////////////////////////////////////////////////////////////////////////////
	//TODO THIS IS WEIRD
	//QList<QStringList> protocol_last = visitTree(ui_p_editor->treeWidget_macroTable);
	/////////////////////////////////////////////////////////////////////////////////


	// all the items 
	std::vector<protocolTreeWidgetItem*> commands_vector;
	//TODO: this is not the best way, 
	//      every time it will re-build the whole macro 
	//      instead of update (waste of time)
	m_protocol->clear();

	// push all the items in the macro table into the command vector
	for (int i = 0;
		i < ui->treeWidget_macroTable->topLevelItemCount();
		++i) {

		// get the current item
		protocolTreeWidgetItem *item =
			dynamic_cast<protocolTreeWidgetItem * > (
				ui->treeWidget_macroTable->topLevelItem(i));
		ui->treeWidget_macroTable->blockSignals(true);
		item->setText(m_cmd_idx_c, QString::number(i+1));//TODO
		ui->treeWidget_macroTable->blockSignals(false);
		//item->blockSignals(true);
		//item->checkValidity(m_cmd_value_c);
		//item->blockSignals(false);


		if (item->childCount() < 1) { // if no children, just add the line 
			string a = ui->treeWidget_macroTable->topLevelItem(i)->text(
				m_cmd_command_c).toStdString();

			commands_vector.push_back(
				dynamic_cast<protocolTreeWidgetItem *> (
					ui->treeWidget_macroTable->topLevelItem(i)));

		}
		else
		{// otherwise we need to traverse the subtree

		 //commands_vector.push_back(ui_p_editor->treeWidget_macroTable->topLevelItem(i)); 
		 //TODO: the actual item is the loop and it should not be added, 
		 //      loops are not supported in the API, they are a high-level feature
		 //TODO: here there is a bug, there is no check that the upper level is actually a loop! 
			for (int loop = 0; loop < item->text(m_cmd_value_c).toInt(); loop++) {
				// we need to check how many times we need to run the operations
				// and add the widget to the list
				for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
					commands_vector.push_back(
						dynamic_cast<protocolTreeWidgetItem *> (
							item->child(childrenCount)));
					ui->treeWidget_macroTable->blockSignals(true);
					protocolTreeWidgetItem *item_child =
						dynamic_cast<protocolTreeWidgetItem *> (
							ui->treeWidget_macroTable->topLevelItem(i)->child(childrenCount));//
					item_child->setText(
						m_cmd_idx_c, QString::number(childrenCount+1));
					ui->treeWidget_macroTable->blockSignals(false);
					//item_child->blockSignals(true);
					//item_child->checkValidity(m_cmd_value_c);
					//item_child->blockSignals(false);
				}
			}
		}
	}

	for (size_t i = 0; i < commands_vector.size(); ++i)
	{

		fluicell::PPC1api::command new_command;

		string a = commands_vector.at(i)->text(m_cmd_command_c).toStdString();

		new_command.setInstruction(static_cast<fluicell::PPC1api::command::instructions>(
			commands_vector.at(i)->text(m_cmd_command_c).toInt()));

		new_command.setValue(commands_vector.at(i)->text(m_cmd_value_c).toInt());
		//new_command.setVisualizeStatus( commands_vector.at(i)->checkState(m_cmd_msg_c)); //TODO clean checkState
		new_command.setStatusMessage(commands_vector.at(i)->text(m_cmd_msg_c).toStdString());

		m_protocol->push_back(new_command);
	}

	// add the protocol to the stack


	// update duration
	double duration = protocolDuration(*m_protocol);
	ui->treeWidget_params->topLevelItem(8)->setText(1, QString::number(duration));
	int remaining_time_sec = duration;
	QString s;
	s.append(m_str_protocol_duration);
	int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	s.append(" h,   ");
	s.append(QString::number(remaining_mins));
	s.append(" min,   ");
	s.append(QString::number(remaining_secs));
	s.append(" sec   ");
	ui->label_protocolDuration->setText(s);
}

double Labonatip_GUI::protocolDuration(std::vector<fluicell::PPC1api::command> _protocol)
{
	// compute the duration of the protocol
	double duration = 0.0;
	for (size_t i = 0; i < _protocol.size(); i++) {
		if (_protocol.at(i).getInstruction() ==
			fluicell::PPC1api::command::instructions::wait)
			duration += _protocol.at(i).getValue();
	}

	return duration;
}

void Labonatip_GUI::protocolsMenu(const QPoint & _pos)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_tools::protocolsMenu " << endl;

	m_triggered_protocol_item = //a class member is used to pass a data between functions
		ui->treeWidget_protocol_folder->indexAt(_pos).row();


	QAction *delete_protocol = new QAction(
		QIcon(":/icons/delete.png"), tr("&Delete"), this);
	delete_protocol->setStatusTip(tr("new sth"));
	connect(delete_protocol, SIGNAL(triggered()),
		this, SLOT(deleteProtocol()));

	QAction *help = new QAction(
		QIcon(":/icons/about.png"), tr("&Help"), this);
	help->setStatusTip(tr("new sth"));
	connect(help, SIGNAL(triggered()), this, SLOT(helpTriggered()));

	QMenu menu(this);
	menu.addAction(delete_protocol);
	menu.addAction(help);

	QPoint pt(_pos);
	menu.exec(ui->treeWidget_protocol_folder->mapToGlobal(_pos));

}

void Labonatip_GUI::helpTriggered() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::helpTriggered   " << endl;
	if (1)
	{
		//this->showUndoStack(); //TODO
		return;

	}
	else
	{
		this->about();
	}


}

void Labonatip_GUI::deleteProtocol()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_tools::deleteProtocol " << endl;

	// TODO: this is not safe as the member could be modified somewhere else
	int row = m_triggered_protocol_item;

	QString file_path = m_protocol_path;
	file_path.append(
		ui->treeWidget_protocol_folder->topLevelItem(row)->text(0));

	QMessageBox::StandardButton resBtn = //TODO: translation
		QMessageBox::question(this, m_str_warning, m_str_remove_file,
			QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes) {
		// continue with file removal 

		QFile f(file_path);
		if (f.exists())
		{
			// delete file
			f.remove();
			// update the folder
			readProtocolFolder(m_protocol_path);
		}
		else
		{
			// the file does not exists
			return;
		}
	}
	else {
		// the choice was no, nothing happens
		return;
	}
}

void Labonatip_GUI::addCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::addMacroCommand " << endl;

	// create the command
	addProtocolCommand *new_command;
	int row = 0;
	//if we are at the top level with no element or no selection 
	// the element is added at the last position
	if (!ui->treeWidget_macroTable->currentIndex().isValid())
	{
		row = ui->treeWidget_macroTable->topLevelItemCount();
		new_command = new addProtocolCommand(
			ui->treeWidget_macroTable, row);
	}
	else { 	//else we add the item at the selected row
		row = ui->treeWidget_macroTable->currentIndex().row() + 1;

		// get the parent
		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem * >(
				ui->treeWidget_macroTable->currentItem()->parent());
		new_command = new addProtocolCommand(
			ui->treeWidget_macroTable, row, parent);
	}

	// add the new command in the undo stack
	m_undo_stack->push(new_command);

	// focus is give to the new added element
	ui->treeWidget_macroTable->setCurrentItem(
		new_command->item(), m_cmd_value_c,
		QItemSelectionModel::SelectionFlag::Rows);

	// every time we add a new command we update all macro commands
	// this is not really nice, better to append (much faster)
	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
}

void Labonatip_GUI::removeCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::removeMacroCommand    " << endl;

	// avoid crash is no elements in the table or no selection
	if (ui->treeWidget_macroTable->currentItem() &&
		ui->treeWidget_macroTable->topLevelItemCount() > 0) {
		removeProtocolCommand *cmd;

		// get the current row
		int row = ui->treeWidget_macroTable->currentIndex().row();

		protocolTreeWidgetItem * parent = dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem()->parent());
		cmd = new removeProtocolCommand(
			ui->treeWidget_macroTable, row, parent);
		m_undo_stack->push(cmd);
	}

	// every time we remove a command we update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
	//updateChartProtocol(m_protocol); //TODO
}


void Labonatip_GUI::moveUp()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::moveUp    " << endl;

	// get the current selected item
	protocolTreeWidgetItem *move_item =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());
	int row = ui->treeWidget_macroTable->currentIndex().row();

	// if the selection is valid and we are not at the first row
	if (move_item && row > 0)
	{
		moveUpCommand *cmd;

		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem *> (
				ui->treeWidget_macroTable->currentItem()->parent());

		// if we are not at the fist level, so the item has a parent
		if (parent) {
			cmd = new moveUpCommand(ui->treeWidget_macroTable,
				parent);
		}
		else {
			// if we are on the top level, just take the item 
			// and add the selected item one row before
			cmd = new moveUpCommand(ui->treeWidget_macroTable);
		}
		m_undo_stack->push(cmd);
		ui->treeWidget_macroTable->setCurrentItem(move_item);
		ui->treeWidget_macroTable->setCurrentItem(
			move_item, m_cmd_value_c, QItemSelectionModel::SelectionFlag::Rows);
	}

	// update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
	//updateChartProtocol(m_protocol); //TODO
}

void Labonatip_GUI::moveDown()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::moveDown    " << endl;

	// get the current selected item
	protocolTreeWidgetItem *move_item =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());
	int row = ui->treeWidget_macroTable->currentIndex().row();
	int number_of_items = ui->treeWidget_macroTable->topLevelItemCount();

	if (move_item && row >= 0 && row < number_of_items - 1)
	{
		moveDownCommand *cmd;
		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem *> (
				ui->treeWidget_macroTable->currentItem()->parent());

		// if the item has a parent
		if (parent) {
			cmd = new moveDownCommand(ui->treeWidget_macroTable,
				parent);
		}
		else {
			cmd = new moveDownCommand(ui->treeWidget_macroTable);
		}

		m_undo_stack->push(cmd);
		ui->treeWidget_macroTable->setCurrentItem(move_item);
		ui->treeWidget_macroTable->setCurrentItem(
			move_item, m_cmd_value_c, QItemSelectionModel::SelectionFlag::Rows);
	}

	// update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
	//updateChartProtocol(m_protocol); //TODO
}

void Labonatip_GUI::plusIndent()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::plusIndent    " << endl;

	// create the command
	addProtocolCommand *cmd;

	// if no item selected, add to the top level
	if (!ui->treeWidget_macroTable->currentIndex().isValid())
	{
		cmd = new addProtocolCommand(ui->treeWidget_macroTable, 0);
	}
	else { // otherwise add the item as a child
		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem *> (
				ui->treeWidget_macroTable->currentItem());

		if (parent->QTreeWidgetItem::parent()) {
			QMessageBox::warning(this, m_str_warning,
				QString("Currently only one loop level is supported"));
			return; //TODO: fix this --- translate string
		}

		if (parent) {

			// set the current parent to be a loop
			parent->setText(m_cmd_command_c, QString::number(17));

			// add the new line as a child
			cmd = new addProtocolCommand(
				ui->treeWidget_macroTable, 0, parent);
		}

	}

	// add the new command in the undo stack
	m_undo_stack->push(cmd);

	// update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
	//updateChartProtocol(m_protocol); //TODO
}

bool Labonatip_GUI::itemChanged(QTreeWidgetItem *_item, int _column)
{
	// check validity for the element
	//cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "Labonatip_GUI::itemChanged    " << endl;

	if (_column == m_cmd_idx_c || _column == m_cmd_range_c)
	{
		dynamic_cast<protocolTreeWidgetItem *>(_item)->checkValidity(_column);
		return true;
	}
	else
	{
		// if the changed element has a parent 
		if (_item->parent())
		{
			changedProtocolCommand * cmd =
				new changedProtocolCommand(ui->treeWidget_macroTable,
					dynamic_cast<protocolTreeWidgetItem *>(_item), _column,
					dynamic_cast<protocolTreeWidgetItem *>(_item->parent()));

			// push the command into the stack
			m_undo_stack->push(cmd);
		}
		else
		{
			// the command is called with null pointer on the parent
			changedProtocolCommand * cmd =
				new changedProtocolCommand(
					ui->treeWidget_macroTable,
					dynamic_cast<protocolTreeWidgetItem *>(_item),
					_column);

			// push the command into the stack
			m_undo_stack->push(cmd);
		}
		dynamic_cast<protocolTreeWidgetItem *>(_item)->checkValidity(_column);
	}

	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
	return true;
}


void Labonatip_GUI::duplicateItem()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::duplicateItem    " << endl;

	// avoid crash if no selection
	if (!ui->treeWidget_macroTable->currentItem()) return;

	// get the current item to clone
	protocolTreeWidgetItem *to_clone =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());

	int command_idx = to_clone->text(m_cmd_command_c).toInt();
	int value = to_clone->text(m_cmd_value_c).toInt();
	//Qt::CheckState show_msg = to_clone->checkState(m_cmd_msg_c);
	QString msg = to_clone->text(m_cmd_msg_c);

	this->addCommand();

	// get the clone, I am aware that the function give the new focus to the added item
	protocolTreeWidgetItem *clone =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());


	clone->setText(m_cmd_command_c, QString::number(command_idx));
	clone->setText(m_cmd_value_c, QString::number(value));
	//clone->setCheckState(m_cmd_command_c, show_msg);
	clone->setText(m_cmd_msg_c, msg);

	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
	return;
}


void Labonatip_GUI::createNewLoop()
{
	this->createNewLoop(2);  // set 2 loops as minimum value ?
}

void Labonatip_GUI::createNewLoop(int _loops)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::createNewLoop    " << endl;

	this->addCommand();
	ui->treeWidget_macroTable->currentItem()->setText(
		m_cmd_command_c, QString::number(17));// "Loop"); // 

	this->plusIndent();
	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
	return;
}

void Labonatip_GUI::clearAllCommandsRequest()
{
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information, 
			m_str_clear_commands + "<br>" + m_str_areyousure,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		// do nothing
	}
	else {
		clearAllCommands();
	}
}

void Labonatip_GUI::clearAllCommands() {
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::clearAllCommands    " << endl;

	ui->treeWidget_macroTable->clear();
	m_protocol->clear();
	m_undo_stack->clear();
}



void Labonatip_GUI::showUndoStack()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showUndoStack " << endl;
	//if (ui->actionShowStack->isChecked())
	//{
	m_undo_view->show();
	//}
	//else
	//{
	//	m_undo_view->hide();
	//}
}

void Labonatip_GUI::undo()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::undo " << endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->undo();
	ui->treeWidget_macroTable->blockSignals(false);
	addAllCommandsToProtocol();

}

void Labonatip_GUI::redo()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::redo " << endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->redo();
	ui->treeWidget_macroTable->blockSignals(false);
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

		if (m_macroRunner_thread->isRunning()) {
			//this->runMacro(); // this will stop the macro if running
			QMessageBox::question(this, m_str_information, m_str_protocol_running_stop, m_str_ok);
			event->ignore();
			return;
		}
		// dump log file
		if (m_GUI_params->dumpHistoryToFile)
		{
			// save log data, messages from the console ect. 
			dumpLogs();
		}
		if (m_ppc1->isConnected()) {
			
			m_ppc1->stop();
			m_ppc1->disconnectCOM(); //if is active, disconnect
		}
		delete m_dialog_tools;
		//delete m_dialog_p_editor;
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
	


  delete qout;
  delete qerr;
  delete m_comSettings;
  delete m_pr_params;
  delete m_GUI_params;
  delete m_pipette_status;
  //delete m_protocol; //TODO: protocol editor and main GUI share the same memory
  delete m_ppc1;
  delete m_macroRunner_thread;
  delete m_update_flowing_sliders;
  delete m_update_GUI;
  delete m_update_waste;
  delete m_scene_solution;
  delete painter_led_green;
  delete led_green;
  delete painter_led_red;
  delete led_red;
  delete m_g_spacer;
  delete m_a_spacer; 
  delete m_labonatip_chart_view;

  delete m_combo_delegate;
  delete m_no_edit_delegate;
  delete m_no_edit_delegate2;
  delete m_spinbox_delegate;
  delete m_reader;
  delete m_writer;

  delete ui;
  qApp->quit();
}
