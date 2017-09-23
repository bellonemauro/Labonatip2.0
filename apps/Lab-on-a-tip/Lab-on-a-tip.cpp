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
	c_x(50.0),
	c_y(45.0),
	c_radius(10.0),
	g_radius(70.0),
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
	m_sol4_color(QColor::fromRgb(130, 255, 0)),//83, 155, 81))
	m_widget_solutionArrow_x_pos (400),
	m_widget_solutionArrow_x_pos_shift (55)
{
  // allows to use path alias
  QDir::setSearchPaths("icons", QStringList(QDir::currentPath() + "/icons/"));
  
  // setup the user interface
  ui->setupUi (this);
  ui->dockWidget->close();  //close the advaced dock page
  ui->treeWidget_macroInfo->setHeaderHidden(false);
  ui->treeWidget_macroInfo->resizeColumnToContents(0);

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

  // all the connects are in this function
  initConnects();
  
  ui->label_emptyTime->setText(" ");

  // status bar to not connected
  ui->statusBar->showMessage("STATUS: NOT Connected  ");

  // hide the warning label
  m_pmap_okIcon = new QPixmap();
  m_pmap_warningIcon = new QPixmap();
  m_pmap_okIcon->load("./icons/okIcon.png");
  m_pmap_warningIcon->load("./icons/warning.png");
  ui->label_warningIcon->clear();
  ui->label_warning->clear();
  //ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
  //ui->label_warningIcon->hide();
  //ui->label_warning->hide();
  //ui->label_warning->setText(" ok ! ");

  // move the arrow in the drawing to point at the solution 1
  ui->widget_solutionArrow->setVisible(false);
  ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol1);
  ui->widget_solutionArrow->move(
	  QPoint( m_widget_solutionArrow_x_pos, 
			  ui->widget_solutionArrow->pos().ry()));
   
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
 
  // not yet used - it can save log data, messages from the console ect. 
  Log_file_name = QString("./Ext_data/LogFile_");

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

  //close the dock tool at inizialization
  //closeOpenDockTools();
  //this->resize(QSize(this->minimumWidth(), this->minimumHeight()));


  // init thread macroRunner //TODO: this is just a support, check if needed
  //Labonatip_macroRunner *m_macroRunner_thread = new Labonatip_macroRunner( this );
  m_macroRunner_thread = new Labonatip_macroRunner(this);
  m_macroRunner_thread->setDevice(m_ppc1);

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

  m_labonatip_chart_view = new Labonatip_chart();
  m_chartView = m_labonatip_chart_view->getChartView();
  ui->gridLayout_12->addWidget(m_chartView);

  ui->actionSimulation->setChecked(true);
  m_simulationOnly = ui->actionSimulation->isChecked();

  m_labonatip_chart_view->setSolutionColor1(m_sol1_color);
  m_labonatip_chart_view->setSolutionColor2(m_sol2_color);
  m_labonatip_chart_view->setSolutionColor3(m_sol3_color);
  m_labonatip_chart_view->setSolutionColor4(m_sol4_color);

}




void Labonatip_GUI::openFile() {
	
	cout << QDate::currentDate().toString().toStdString() << "  "
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::openFile    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QString _path = QFileDialog::getOpenFileName (this, tr("Open Settings file"), QDir::currentPath(),  // dialog to open files
						"Settings file (*.ini);; All Files(*.*)" , 0);

	if (_path.isEmpty()) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "Empty path, file not found ! <br>" + _path);
		return;
	}

	if (!m_dialog_tools->setLoadSettingsFileName(_path)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "Cannot load the file ! <br>" + _path);
		return;
	}

   QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

}

void Labonatip_GUI::saveFile() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::saveFile    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	/*if (something->isEmpty()) 
	{QMessageBox::warning(this, "Warning !", "something empty cannot not saved  ! " );
	}*/
	QString _path = QFileDialog::getSaveFileName (this, tr("Save configuration file"), QDir::currentPath(),  // dialog to open files
						"Settings file (*.ini);; All Files(*.*)" , 0);
	
	if (_path.isEmpty()) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "Empty path, file not saved ! <br>" + _path);
		return;
	}

	if (!m_dialog_tools->setSaveSettingsFileName(_path)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "Cannot save the file ! <br>" + _path);
		return;
	}

  
   QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}


void Labonatip_GUI::showToolsDialog() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::showToolsDialog    " << endl;

	m_dialog_tools->setMacroPath(m_macro_path); //TODO: reset the macro path in case it is changed in the settings

	m_dialog_tools->setWindowFlags(Qt::WindowFullscreenButtonHint);
	m_dialog_tools->setModal(false);
	m_macro = new std::vector<fluicell::PPC1api::command>();
	m_dialog_tools->setMacroPrt(m_macro);
	m_dialog_tools->show();

}




void Labonatip_GUI::disCon() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::disCon    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

    if (m_simulationOnly) 	{ 
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::information(this, "Warning !", "Lab-on-a-tip is in simulation only  ");
		return;
	}

	try
	{

		if (!m_ppc1->isRunning()) {

			if (!m_ppc1->isConnected())
				if (!m_ppc1->connectCOM()) { 
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

			m_ppc1->run();   // TODO: this is not the best way of running the device as it cannot handle exeptions
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

void Labonatip_GUI::colSolution1Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol1_color.setRgb(_r, _g, _b);
	ui->progressBar_solution1->setStyleSheet(styleSheet);
	m_labonatip_chart_view->setSolutionColor1(m_sol1_color);
}

void Labonatip_GUI::colSolution2Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol2_color.setRgb(_r, _g, _b);
	ui->progressBar_solution2->setStyleSheet(styleSheet);
	m_labonatip_chart_view->setSolutionColor2(m_sol2_color);
}


void Labonatip_GUI::colSolution3Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol3_color.setRgb(_r, _g, _b);
	ui->progressBar_solution3->setStyleSheet(styleSheet);
	m_labonatip_chart_view->setSolutionColor3(m_sol3_color);
}


void Labonatip_GUI::colSolution4Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol4_color.setRgb(_r, _g, _b);
	ui->progressBar_solution4->setStyleSheet(styleSheet);
	m_labonatip_chart_view->setSolutionColor4(m_sol4_color);
	//ui->progressBar_solution4->setPalette(*palette);
}

QString Labonatip_GUI::generateStyleSheet(const int _r, const int _g, const int _b)
{
	
	QString styleSheet(" QProgressBar{	border: 1px solid white;");
	styleSheet.append("padding: 1px;");
	styleSheet.append("color: rgb(255, 255, 255, 0); ");
	styleSheet.append("border-bottom-right-radius: 2px;");
	styleSheet.append("border-bottom-left-radius: 2px;");
	styleSheet.append("border-top-right-radius: 2px;");
	styleSheet.append("border-top-left-radius: 2px;");
	styleSheet.append("text-align:right;");
	//	margin-right: 25ex;
	styleSheet.append("background-color: rgb(255, 255, 255, 0);");
	styleSheet.append("width: 15px;}");

	styleSheet.append("QProgressBar::chunk{");
	styleSheet.append("background-color: rgb(");
	styleSheet.append(QString::number(_r));
	styleSheet.append(", ");
	styleSheet.append(QString::number(_g));
	styleSheet.append(", ");
	styleSheet.append(QString::number(_b));
	styleSheet.append("); ");
	styleSheet.append("border-bottom-right-radius: 2px;");
	styleSheet.append("border-bottom-left-radius: 2px;");
	styleSheet.append("border-top-right-radius: 2px;");
	styleSheet.append("border-top-left-radius: 2px;");
	styleSheet.append("border: 0px solid white;");
	styleSheet.append("height: 0.5px;}");

	return styleSheet;
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

void Labonatip_GUI::stopSolutionFlow()
{
	// look for the active flow
	if (ui->pushButton_solution1->isChecked()) {
		ui->pushButton_solution1->setChecked(false);
		pushSolution1(); // if the flow is active, this should stop it!

		updateDrawing(-1);
		return;
	}
	if (ui->pushButton_solution2->isChecked()) {
		ui->pushButton_solution2->setChecked(false);
		pushSolution2();
		updateDrawing(-1);
		return;
	}
	if (ui->pushButton_solution3->isChecked()) {
		ui->pushButton_solution3->setChecked(false);
		pushSolution3();
		updateDrawing(-1);
		return;
	}
	if (ui->pushButton_solution4->isChecked()) {
		ui->pushButton_solution4->setChecked(false);
		pushSolution4();
		updateDrawing(-1);
		return;
	}
	// if none is checked, do nothing.




	return;
}

void Labonatip_GUI::setEnableSolutionButtons(bool _enable ) {
	ui->pushButton_solution1->setEnabled(_enable);
	ui->pushButton_solution2->setEnabled(_enable);
	ui->pushButton_solution3->setEnabled(_enable);
	ui->pushButton_solution4->setEnabled(_enable);
}


void Labonatip_GUI::closeOpenDockTools() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::closeOpenDockTools   " << endl;

	// get the screen resolution of the current screen
	// so we can resize the application in case of small screens
	//QRect rec = QApplication::desktop()->screenGeometry();
	//int screen_height = rec.height();
	//int screen_width = rec.width();

	if (!ui->dockWidget->isHidden()) {
		ui->dockWidget->hide();
		if (!this->isMaximized())
			this->resize(QSize(this->width(), this->height()));//	this->resize(QSize(this->minimumWidth(), this->height()));
	}
	else {

		if (this->width() < this->minimumWidth() + ui->dockWidget->width())
		{
			ui->dockWidget->show();
			if (!this->isMaximized())
				this->resize(QSize(this->width() + ui->dockWidget->width(), this->height()));
		}
		else
		{
		ui->dockWidget->show();
		if (!this->isMaximized())
			this->resize(QSize(this->width(), this->height()));
		}
		
	}

}


void Labonatip_GUI::updateDrawing( int _value) {


	if (_value == -1) { // _value = -1 cleans the scene and make the flow disappear 

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_GUI::updateDrawing :::: clear scene  " << endl;
		m_scene_solution->clear();
// TODO: for some reason here the scene is not removed 
		m_pen_line.setColor(Qt::lightGray);
		QBrush brush(m_pen_line.color(), Qt::SolidPattern);

		m_scene_solution->addEllipse(c_x, c_y - _value / 2,
			c_radius + _value, c_radius + _value,
			m_pen_flow, brush);

		// draw a line from the injector to the solution release point 
		m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);

		ui->graphicsView->setScene(m_scene_solution);
		ui->graphicsView->update();
		ui->graphicsView->show();
		return;
	}

	//if( ui->pushButton_solution1->isChecked() || 
	//	ui->pushButton_solution2->isChecked() ||
	//	ui->pushButton_solution3->isChecked() ||
	//	ui->pushButton_solution4->isChecked())
	//{
	
	//clean the scene
	m_scene_solution->clear();

	_value = _value / 2;
	// draw the circle 
	//QBrush brush(*m_gradient_flow);
	QBrush brush(m_pen_line.color(), Qt::SolidPattern);


	//m_scene_solution->addEllipse( c_x, c_y - _value/8.0, 
	//	                          c_radius + _value/4.0, c_radius + _value/4.0, 
	//							  m_pen_flow, brush );
	
	QPen * penna = new QPen();
	penna->setColor(Qt::transparent);
	penna->setWidth(1);
	//m_scene_solution->addLine(l_x2 - 20, l_y2 - 20, l_x2 + 20, l_y2 + 20, *penna);

	// TODO: this is an attempt to make the droplet to look a little bit more realistic
	QPainterPath* path = new QPainterPath();
	path->arcMoveTo(45, 34, 20, 20, -90);
	path->arcTo(45, 36, 15 + _value / 8.0, 25 +_value / 50.0, -90 - _value / 5.0, 180 + _value / 2.5);
	path->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*path, *penna, brush);

	// draw a line from the injector to the solution release point 
	m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);

	ui->graphicsView->setScene(m_scene_solution);
	ui->graphicsView->show();
	//}

	return;

}


void Labonatip_GUI::pushSolution1()
{
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pushSolution1    " << endl;

	if (!ui->pushButton_solution1->isChecked()){ // this allows to stop the flow when active
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		//m_update_time_s1->stop();
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::lightGray);

		return;
	}

	// stop all other valves
	if (ui->pushButton_solution2->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution2->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution3->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution3->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution4->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol1_color);
	m_flowing_solution = 1;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	// move the arrow in the drawing to point on the solution 1
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol1);
	ui->widget_solutionArrow->move(
		QPoint( m_widget_solutionArrow_x_pos, 
			    ui->widget_solutionArrow->pos().ry()));

	// Here start the solution flow 
	double solution_release_time = m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;


	// SET vacum to _value
	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_l(true);
	}
	m_timer_solution = 0;
	m_update_flowing_sliders->start();

//	setEnableSolutionButtons(false);

}

void Labonatip_GUI::pushSolution2() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pushSolution2   " << endl;
	
	if (!ui->pushButton_solution2->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::lightGray);

		return;
	}
	
	if (ui->pushButton_solution1->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution1->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution3->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution3->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution4->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol2_color);
	m_flowing_solution = 2;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	// move the arrow in the drawing to point on the solution 2
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol2);
	ui->widget_solutionArrow->move(
		QPoint( m_widget_solutionArrow_x_pos - m_widget_solutionArrow_x_pos_shift, // shift
			    ui->widget_solutionArrow->pos().ry()));

	double solution_release_time = m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;
	
	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_k(true);
	}

	m_timer_solution = 0;
	m_update_flowing_sliders->start();
	//setEnableSolutionButtons(false);

}

void Labonatip_GUI::pushSolution3() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pushSolution3   " << endl;


	if (!ui->pushButton_solution3->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::lightGray);

		return;
	}

	if (ui->pushButton_solution1->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution1->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution2->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution2->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution4->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol3_color);
	m_flowing_solution = 3;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	// move the arrow in the drawing to point on the solution 3
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol3);
	//ui->widget_solutionArrow->move(QPoint(290, ui->widget_solutionArrow->pos().ry()));
	ui->widget_solutionArrow->move(
		QPoint(m_widget_solutionArrow_x_pos - 2 * m_widget_solutionArrow_x_pos_shift, // shift
			ui->widget_solutionArrow->pos().ry()));
	//TODO: I don't like the static movement, let's find another solution for it ! 

	double solution_release_time = m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_j(true);
	}

	m_timer_solution = 0;
	m_update_flowing_sliders->start();
	//setEnableSolutionButtons(false);

}

void Labonatip_GUI::pushSolution4() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pushSolution4   " << endl;


	if (!ui->pushButton_solution4->isChecked()){
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::lightGray);

		return;
	}

	ui->pushButton_solution1->setChecked(false);
	if (ui->pushButton_solution1->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution1->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution2->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution2->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution3->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution3->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol4_color);
	m_flowing_solution = 4;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	// move the arrow in the drawing to point on the solution 4
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_dialog_tools->m_solutionNames->sol4);
	//ui->widget_solutionArrow->move(QPoint(235, ui->widget_solutionArrow->pos().ry()));
	ui->widget_solutionArrow->move(
		QPoint(m_widget_solutionArrow_x_pos - 3 * m_widget_solutionArrow_x_pos_shift, // shift
			ui->widget_solutionArrow->pos().ry()));
	//TODO: I don't like the static movement, let's find another solution for it ! 

	double solution_release_time = m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	if (m_pipette_active)
	{
		m_ppc1->closeAllValves();
		QThread::msleep(50);
		m_ppc1->setValve_i(true);
	}

	m_timer_solution = 0;
	m_update_flowing_sliders->start();
	//setEnableSolutionButtons(false);

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


void Labonatip_GUI::resetWells() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::resetWells   " << endl;


	ui->progressBar_solution1->setValue(100);
	ui->progressBar_solution2->setValue(100);
	ui->progressBar_solution3->setValue(100);
	ui->progressBar_solution4->setValue(100);
}


void Labonatip_GUI::updateTimingSliders( )
{
	QProgressBar *_bar;
	QPushButton *_button;

	switch (m_flowing_solution)
	{
	case 1 :  {
		_bar = ui->progressBar_solution1;
		_button = ui->pushButton_solution1;
		break; 
	}
	case 2:  {
		_bar = ui->progressBar_solution2; 
		_button = ui->pushButton_solution2;
		break;
	}
	case 3: {
		_bar = ui->progressBar_solution3; 
		_button = ui->pushButton_solution3;
		break;
	}
	case 4: {
		_bar = ui->progressBar_solution4;
		_button = ui->pushButton_solution4;
		break;
	}
	default : {
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_GUI::updateTimingSliders  error --- no valid m_flowing_solution value " << endl;
		return;
	}
	}


	if (m_timer_solution < m_time_multipilcator) {
		m_update_flowing_sliders->start();
		int status = int(100 * m_timer_solution / m_time_multipilcator);
		//ui->widget_sol1->setValue(status);
		_bar->setValue(100 - status);
		//m_labonatip_chart_view->updateChartTime(status); //TODO: take this out of here
		QString s;
		if (!m_dialog_tools->isContinuousFlowing()) {
			s.append("Empty in ");
			int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
			int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
			int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
			s.append(QString::number(remaining_hours));
			s.append(" h, ");
			s.append(QString::number(remaining_mins));
			s.append(" min ");
		}
		else
		{
			s.append("Continuous flowing");
		}
		ui->label_emptyTime->setText(s);
		//ui->doubleSpinBox_solution->setValue(remaining_time_in_sec);
		m_timer_solution++;
		// show the warning label

		if (m_pipette_active) updateDrawing(m_ppc1->getDropletSize());
		else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

		//ui->label_warningIcon->show();
		//ui->label_warning->show();
		if (status > 50) {
			ui->label_warningIcon->setPixmap(*m_pmap_warningIcon);
			ui->label_warning->setText(" warning solution is ending ");
		}
		return;
	}
	else  // here we are ending the release process of the solution
	{
		if (m_dialog_tools->isContinuousFlowing())//(ui->checkBox_disableTimer->isChecked() ) // TODO: bring the param to settings
		{
			m_update_flowing_sliders->start();
			QString s;
			s.append("Continuous flowing");
			ui->label_emptyTime->setText(s);
			return;
		}

		double solution_release_time = m_dialog_tools->getSolutionTime();
		m_time_multipilcator = (int)solution_release_time;
		double rest = solution_release_time - m_time_multipilcator;
		QThread::msleep(rest*1000);
		// TODO: here we wait the remaing time for the last digit
		//       however, this is a shitty solution and it must be
		//       changed to a proper timer and interrupt architecture

		m_update_flowing_sliders->stop();
		m_timer_solution = 0;
		_bar->setValue(10); //set the minimum just to visualize something
		//ui->widget_sol1->setValue(100);
		if (m_pipette_active)
		{
			//m_ppc1->setValve_l(false);
			m_ppc1->closeAllValves();
		}
		setEnableSolutionButtons(true);
		_button->setChecked(false);
		ui->widget_solutionArrow->setVisible(false);
		updateDrawing(-1); // remove the droplet from the drawing



		ui->label_warningIcon->clear();
		ui->label_warning->clear();
		//ui->label_warningIcon->setPixmap(*m_pmap_okIcon);
		//ui->label_warning->setText(" Ok! ");
		//ui->label_warningIcon->hide();
		//ui->label_warning->hide();

		return;
	}

}

void Labonatip_GUI::updateFlows()
{

	// calculate the flow
	double delta_pressure = 0.0;
	double pipe_length = 0.124;
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
		ui->treeWidget_macroInfo->topLevelItem(1)->setText(1, QString::number(m_ppc1->m_PPC1_status->outflow));
		ui->treeWidget_macroInfo->topLevelItem(2)->setText(1, QString::number(m_ppc1->m_PPC1_status->inflow_recirculation));
		ui->treeWidget_macroInfo->topLevelItem(3)->setText(1, QString::number(m_ppc1->m_PPC1_status->in_out_ratio));

		ui->treeWidget_macroInfo->topLevelItem(6)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_1));
		ui->treeWidget_macroInfo->topLevelItem(7)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_2));
		ui->treeWidget_macroInfo->topLevelItem(8)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_3));
		ui->treeWidget_macroInfo->topLevelItem(9)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_4));
		ui->treeWidget_macroInfo->topLevelItem(10)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_5));
		ui->treeWidget_macroInfo->topLevelItem(11)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_6));
		ui->treeWidget_macroInfo->topLevelItem(12)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_7));
		ui->treeWidget_macroInfo->topLevelItem(13)->setText(1, QString::number(m_ppc1->m_PPC1_status->flow_rate_8));

		return;
	}
	else{
		v_s = m_v_switch_set_point;
		p_on = m_pon_set_point;
		p_off = m_poff_set_point;
	}
	// calculate inflow
	delta_pressure = 100.0 * v_s;

	pipe_length = 0.065;
	inflow_recirculation = 2 * m_ppc1->getFlowSimple(delta_pressure, pipe_length);

	delta_pressure = 100.0 * (v_s + 2 * p_off *  0.046153846);
	inflow_switch = 2 * m_ppc1->getFlowSimple(delta_pressure, pipe_length);

	delta_pressure = 100.0 * 2 * p_off;
	pipe_length = 0.124;
	solution_usage_off = m_ppc1->getFlowSimple(delta_pressure, pipe_length);

	delta_pressure = 100.0 * p_on;
	pipe_length = 0.065;
	solution_usage_on = m_ppc1->getFlowSimple(delta_pressure, pipe_length);


	if (ui->pushButton_solution1->isChecked() ||
		ui->pushButton_solution2->isChecked() ||
		ui->pushButton_solution3->isChecked() ||
		ui->pushButton_solution4->isChecked()) // flow when solution is off
		{

		delta_pressure = 100.0 * (p_on +
			(p_off * 3) -
			(v_s * 2));

		pipe_length = 0.065;
		outflow = m_ppc1->getFlowSimple( delta_pressure, pipe_length);



		flow_rate_1 = solution_usage_on;
		flow_rate_2 = solution_usage_off;
		flow_rate_3 = solution_usage_off;
		flow_rate_4 = solution_usage_off;
		flow_rate_5 = inflow_switch / 2.0;
		flow_rate_6 = inflow_switch / 2.0;
		flow_rate_7 = inflow_recirculation / 2.0;
		flow_rate_8 = inflow_recirculation / 2.0;

	}
	else // flow when solution is on
	{
		delta_pressure = 100.0 * ((p_off * 4) -
			(v_s * 2));

		pipe_length = 0.124;
		outflow = 2 * m_ppc1->getFlowSimple( delta_pressure, pipe_length);

		flow_rate_1 = solution_usage_off;
		flow_rate_2 = solution_usage_off;
		flow_rate_3 = solution_usage_off;
		flow_rate_4 = solution_usage_off;
		flow_rate_5 = inflow_switch / 2.0;
		flow_rate_6 = inflow_switch / 2.0;
		flow_rate_7 = inflow_recirculation / 2.0;
		flow_rate_8 = inflow_recirculation / 2.0;
	}



	in_out_ratio = std::abs(outflow / inflow_recirculation);



	ui->treeWidget_macroInfo->topLevelItem(1)->setText(1, QString::number(outflow));
	ui->treeWidget_macroInfo->topLevelItem(2)->setText(1, QString::number(inflow_recirculation));
	ui->treeWidget_macroInfo->topLevelItem(3)->setText(1, QString::number(in_out_ratio));

	ui->treeWidget_macroInfo->topLevelItem(6)->setText(1, QString::number(flow_rate_1));
	ui->treeWidget_macroInfo->topLevelItem(7)->setText(1, QString::number(flow_rate_2));
	ui->treeWidget_macroInfo->topLevelItem(8)->setText(1, QString::number(flow_rate_3));
	ui->treeWidget_macroInfo->topLevelItem(9)->setText(1, QString::number(flow_rate_4));
	ui->treeWidget_macroInfo->topLevelItem(10)->setText(1, QString::number(flow_rate_5));
	ui->treeWidget_macroInfo->topLevelItem(11)->setText(1, QString::number(flow_rate_6));
	ui->treeWidget_macroInfo->topLevelItem(12)->setText(1, QString::number(flow_rate_7));
	ui->treeWidget_macroInfo->topLevelItem(13)->setText(1, QString::number(flow_rate_8));


	return;
}


void Labonatip_GUI::updateGUI() {

	if (!m_simulationOnly) {
		int sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_B->sensor_reading);  // rounded to second decimal
		int set_point = (int)(m_ppc1->m_PPC1_data->channel_B->set_point);
		ui->label_switchPressure->setText(QString(QString::number(sensor_reading) + ", " + QString::number(set_point) + " mbar"));
		ui->progressBar_switch->setValue(-sensor_reading);


		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_A->sensor_reading);
		set_point = (int)(m_ppc1->m_PPC1_data->channel_A->set_point);
		ui->label_recircPressure->setText(QString(QString::number(sensor_reading) + ", " + QString::number(set_point) + " mbar"));
		ui->progressBar_recirc->setValue(-sensor_reading);


		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_C->sensor_reading);
		set_point = (int)(m_ppc1->m_PPC1_data->channel_C->set_point);
		ui->label_PoffPressure->setText(QString(QString::number(sensor_reading) + ", " + QString::number(set_point) + " mbar"));
		ui->progressBar_pressure_p_off->setValue(sensor_reading);


		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_D->sensor_reading);
		set_point = (int)(m_ppc1->m_PPC1_data->channel_D->set_point);
		ui->label_PonPressure->setText(QString(QString::number(sensor_reading) + ", " + QString::number(set_point) + " mbar"));
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


	//if (m_ppc1->isRunning())
  	    m_update_GUI->start();
}


bool Labonatip_GUI::saveLog(QString &_file_name) // This is deprecated
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
		SIGNAL(ok()), this, 
		SLOT(toolOk()));

	connect(m_dialog_tools, 
		SIGNAL(apply()), this, 
		SLOT(toolApply()));

}

void Labonatip_GUI::toolOk() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::toolOk   " << endl;

	m_ppc1->setCOMport(m_dialog_tools->m_comSettings->name);
	m_ppc1->setBaudRate((int)m_dialog_tools->m_comSettings->baudRate);


	/////////////////////////////////////////
	// TODO set all the other options
	if (m_dialog_tools->isContinuousFlowing())
	{
		m_update_flowing_sliders->start();
		QString s;
		s.append("Continuous flowing");
		ui->label_emptyTime->setText(s);
	}
	else
	{
		QString s;
		s.append("Empty in ");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, ");
		s.append(QString::number(remaining_mins));
		s.append(" min ");
	}

	//updateChartMacro();
	m_labonatip_chart_view->updateChartMacro(m_macro);

	// compute the duration of the macro
	double macro_duration = 0.0;
	for (int i = 0; i < m_macro->size(); i++) {
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
		s.append("Continuous flowing");
		ui->label_emptyTime->setText(s);
	}
	else
	{
		QString s;
		s.append("Empty in ");
		int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
		int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
		int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
		s.append(QString::number(remaining_hours));
		s.append(" h, ");
		s.append(QString::number(remaining_mins));
		s.append(" min ");
	}

	m_labonatip_chart_view->updateChartMacro(m_macro);

	// compute the duration of the macro
	double macro_duration = 0.0;
	for (int i = 0; i < m_macro->size(); i++) {
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


void Labonatip_GUI::simulationOnly()
{
	m_simulationOnly = ui->actionSimulation->isChecked();

	ui->actionDisCon->setEnabled(!m_simulationOnly);
	ui->actionRun->setEnabled(!m_simulationOnly);
	ui->actionReset->setEnabled(!m_simulationOnly);
	ui->actionSleep->setEnabled(!m_simulationOnly);
	ui->actionShutdown->setEnabled(!m_simulationOnly);
	m_macroRunner_thread->setSimulationFlag(m_simulationOnly);

	if (m_simulationOnly)ui->treeWidget_macroInfo->topLevelItem(0)->setText(1, "Simulation");
	else ui->treeWidget_macroInfo->topLevelItem(0)->setText(1, "PPC1");

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

	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Lab-on-a-tip",
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
