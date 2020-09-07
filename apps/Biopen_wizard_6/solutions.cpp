/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"


QString Labonatip_GUI::generateStyleSheet(const int _r, const int _g, const int _b)
{

	QString styleSheet(" QProgressBar{	border: 1px solid white;"
	                   " padding: 1px;"
	                   " color: rgb(255, 255, 255, 0); "
	                   " border-bottom-right-radius: 2px;"
	                   " border-bottom-left-radius: 2px;"
	                   " border-top-right-radius: 2px;"
	                   " border-top-left-radius: 2px;"
	                   " text-align:right;"
	                   " background-color: rgb(255, 255, 255, 0);"
	                   " width: 15px;}"
	                   " QProgressBar::chunk{"
	                   " background-color: rgb(");
	styleSheet.append(QString::number(_r));
	styleSheet.append(", ");
	styleSheet.append(QString::number(_g));
	styleSheet.append(", ");
	styleSheet.append(QString::number(_b));
	styleSheet.append("); ");
	styleSheet.append(" border-bottom-right-radius: 2px;"
	                  " border-bottom-left-radius: 2px;"
	                  " border-top-right-radius: 2px;"
	                  " border-top-left-radius: 2px;"
	                  " border: 0px solid white;"
	                  " height: 0.5px;}");

	return styleSheet;
}


void Labonatip_GUI::colSolution1Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol1_color.setRgb(_r, _g, _b);
	ui->progressBar_solution1->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor1(m_sol1_color);
}


void Labonatip_GUI::colSolution2Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol2_color.setRgb(_r, _g, _b);
	ui->progressBar_solution2->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor2(m_sol2_color);
}


void Labonatip_GUI::colSolution3Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol3_color.setRgb(_r, _g, _b);
	ui->progressBar_solution3->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor3(m_sol3_color);
}


void Labonatip_GUI::colSolution4Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol4_color.setRgb(_r, _g, _b);
	ui->progressBar_solution4->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor4(m_sol4_color);
}

void Labonatip_GUI::colSolution5Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol5_color.setRgb(_r, _g, _b);
	ui->progressBar_solution5->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor5(m_sol5_color);
}

void Labonatip_GUI::colSolution6Changed(const int _r, const int _g, const int _b)
{

	QString styleSheet = generateStyleSheet(_r, _g, _b);
	m_sol6_color.setRgb(_r, _g, _b);
	ui->progressBar_solution6->setStyleSheet(styleSheet);
	m_chart_view->setSolutionColor6(m_sol6_color);
}

void Labonatip_GUI::pushSolution1() 
{
	std::cout << HERE << std::endl;

	if (!ui->pushButton_solution1->isChecked()) { // this allows to stop the flow when active
		m_timer_solution = m_time_multipilcator;

		//if (m_pipette_active) 
		{
			//m_ppc1->closeAllValves();

			// stop pumping protocol
			QString currentProtocolFileName = m_protocol_path;
			currentProtocolFileName.append("/");
			currentProtocolFileName.append("stopSolution1.prt");
			this->runProtocolFile(currentProtocolFileName);
		}

		//m_update_time_s1->stop();
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		updateFlowControlPercentages();
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
	if (ui->pushButton_solution5->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution5->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution6->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution6->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol1_color);
	m_flowing_solution = 1;

	// move the arrow in the drawing to point on the solution 1
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol1);
	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution1->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution1->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	// Here start the solution flow 
	double solution_release_time = m_solutionParams->pulse_duration_well1; //    m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;


	// SET vacum to _value
	//if (m_pipette_active)
	{
		//m_ppc1->closeAllValves();
		//QThread::msleep(50);
		//m_ppc1->setValve_l(true);

		// start pumping protocol
		QString currentProtocolFileName = m_protocol_path;
		currentProtocolFileName.append("/");
		currentProtocolFileName.append("pumpSolution1.prt");
		this->runProtocolFile(currentProtocolFileName);
	}
	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution2() {

	std::cout << HERE << std::endl;

	if (!ui->pushButton_solution2->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		//if (m_pipette_active) 
		{
			//m_ppc1->closeAllValves();
			// stop pumping protocol
			QString currentProtocolFileName = m_protocol_path;
			currentProtocolFileName.append("/");
			currentProtocolFileName.append("stopSolution2.prt");
			this->runProtocolFile(currentProtocolFileName);
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		updateFlowControlPercentages();
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
	if (ui->pushButton_solution5->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution5->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution6->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution6->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol2_color);
	m_flowing_solution = 2;

	// move the arrow in the drawing to point on the solution 2
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol2);

	int pos_x = ui->progressBar_solution2->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution2->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	double solution_release_time = m_solutionParams->pulse_duration_well2; //m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	//if (m_pipette_active)
	{
		//m_ppc1->closeAllValves();
		//QThread::msleep(50);
		//m_ppc1->setValve_k(true);

		// start pumping protocol
		QString currentProtocolFileName = m_protocol_path;
		currentProtocolFileName.append("/");
		currentProtocolFileName.append("pumpSolution2.prt");
		this->runProtocolFile(currentProtocolFileName);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution3() {

	std::cout << HERE << std::endl;

	if (!ui->pushButton_solution3->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		//if (m_pipette_active) 
		{
			//m_ppc1->closeAllValves();
			// stop pumping protocol
			QString currentProtocolFileName = m_protocol_path;
			currentProtocolFileName.append("/");
			currentProtocolFileName.append("stopSolution3.prt");
			this->runProtocolFile(currentProtocolFileName);
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		updateFlowControlPercentages();
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
	if (ui->pushButton_solution5->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution5->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution6->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution6->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol3_color);
	m_flowing_solution = 3;

	// move the arrow in the drawing to point on the solution 3
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol3);
	//ui->widget_solutionArrow->move(QPoint(290, ui->widget_solutionArrow->pos().ry()));

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution3->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution3->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	double solution_release_time = m_solutionParams->pulse_duration_well3; //m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	//if (m_pipette_active)
	{
		//m_ppc1->closeAllValves();
		//QThread::msleep(50);
		//m_ppc1->setValve_j(true);

		// start pumping protocol
		QString currentProtocolFileName = m_protocol_path;
		currentProtocolFileName.append("/");
		currentProtocolFileName.append("pumpSolution3.prt");
		this->runProtocolFile(currentProtocolFileName);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution4() {

	std::cout << HERE << std::endl;

	if (!ui->pushButton_solution4->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		//if (m_pipette_active) 
		{
			m_ppc1->closeAllValves();
			// stop pumping protocol
			QString currentProtocolFileName = m_protocol_path;
			currentProtocolFileName.append("/");
			currentProtocolFileName.append("stopSolution4.prt");
			this->runProtocolFile(currentProtocolFileName);
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		updateFlowControlPercentages();
		return;
	}

	//ui->pushButton_solution1->setChecked(false);
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
	if (ui->pushButton_solution5->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution5->setChecked(false);
		m_update_flowing_sliders->stop();
	}
	if (ui->pushButton_solution6->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution6->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol4_color);
	m_flowing_solution = 4;

	// move the arrow in the drawing to point on the solution 4
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol4);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution4->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution4->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	double solution_release_time = m_solutionParams->pulse_duration_well4; //m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	//if (m_pipette_active)
	{
		//m_ppc1->closeAllValves();
		//QThread::msleep(50);
		//m_ppc1->setValve_i(true);
		// start pumping protocol
		QString currentProtocolFileName = m_protocol_path;
		currentProtocolFileName.append("/");
		currentProtocolFileName.append("pumpSolution4.prt");
		this->runProtocolFile(currentProtocolFileName);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution5() {

	std::cout << HERE << std::endl;

	if (!ui->pushButton_solution5->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		//if (m_pipette_active) 
		{
			m_ppc1->closeAllValves();
			// stop pumping protocol
			QString currentProtocolFileName = m_protocol_path;
			currentProtocolFileName.append("/");
			currentProtocolFileName.append("stopSolution5.prt");
			this->runProtocolFile(currentProtocolFileName);
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		updateFlowControlPercentages();
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
	if (ui->pushButton_solution6->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution6->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol5_color);
	m_flowing_solution = 5;

	// move the arrow in the drawing to point on the solution 4
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol5);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution5->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution5->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	double solution_release_time = m_solutionParams->pulse_duration_well5; //m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	//if (m_pipette_active)
	{
		//m_ppc1->closeAllValves();
		//QThread::msleep(50);
		//m_ppc1->setValve_e(true);

		// start pumping protocol
		QString currentProtocolFileName = m_protocol_path;
		currentProtocolFileName.append("/");
		currentProtocolFileName.append("pumpSolution5.prt");
		this->runProtocolFile(currentProtocolFileName);
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution6() {

	std::cout << HERE << std::endl;

	if (!ui->pushButton_solution6->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		//if (m_pipette_active) 
		{
			m_ppc1->closeAllValves();
			// stop pumping protocol
			QString currentProtocolFileName = m_protocol_path;
			currentProtocolFileName.append("/");
			currentProtocolFileName.append("stopSolution6.prt");
			this->runProtocolFile(currentProtocolFileName);
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->textEdit_emptyTime->hide();// setText(" ");
		updateFlowControlPercentages();
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
	if (ui->pushButton_solution5->isChecked()) {
		m_timer_solution = m_time_multipilcator;
		ui->pushButton_solution5->setChecked(false);
		m_update_flowing_sliders->stop();
	}

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol6_color);
	m_flowing_solution = 6;

	// move the arrow in the drawing to point on the solution 6
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol6);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution6->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution5->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

	double solution_release_time = m_solutionParams->pulse_duration_well6; //m_dialog_tools->getSolutionTime();
	m_time_multipilcator = (int)solution_release_time;

	//if (m_pipette_active)
	{
		//m_ppc1->closeAllValves();
		//QThread::msleep(50);
		//m_ppc1->setValve_f(true);

		// start pumping protocol
		QString currentProtocolFileName = m_protocol_path;
		currentProtocolFileName.append("/");
		currentProtocolFileName.append("pumpSolution6.prt");
		//bool suc = 
		if (QFile::exists(currentProtocolFileName))
		{
		this->runProtocolFile(currentProtocolFileName);
		}
		
	}

	m_timer_solution = 0;
	//m_update_flowing_sliders->start();
	updateTimingSliders();

	updateFlows();
	updateFlowControlPercentages();

}


void Labonatip_GUI::updateTimingSliders()
{
	QProgressBar *_bar;
	QPushButton *_button;
	double status;
	bool continuous_flowing = false;
	double release_time = 0.0;

	switch (m_flowing_solution)
	{
	case 1: { 
		_bar = ui->progressBar_solution1;
		_button = ui->pushButton_solution1;
		status = m_pipette_status->rem_vol_well1;
		continuous_flowing = m_solutionParams->continuous_flowing_sol1;
		release_time = m_solutionParams->pulse_duration_well1;
		break;
	}
	case 2: {
		_bar = ui->progressBar_solution2;
		_button = ui->pushButton_solution2;
		status = m_pipette_status->rem_vol_well2;
		continuous_flowing = m_solutionParams->continuous_flowing_sol2;
		release_time = m_solutionParams->pulse_duration_well2;
		break;
	}
	case 3: {
		_bar = ui->progressBar_solution3;
		_button = ui->pushButton_solution3;
		status = m_pipette_status->rem_vol_well3;
		continuous_flowing = m_solutionParams->continuous_flowing_sol3;
		release_time = m_solutionParams->pulse_duration_well3;
		break;
	}
	case 4: {
		_bar = ui->progressBar_solution4;
		_button = ui->pushButton_solution4;
		status = m_pipette_status->rem_vol_well4;
		continuous_flowing = m_solutionParams->continuous_flowing_sol4;
		release_time = m_solutionParams->pulse_duration_well4;
		break;
	}
	case 5: {
		_bar = ui->progressBar_solution5;
		_button = ui->pushButton_solution5;
		status = m_pipette_status->rem_vol_well5;
		continuous_flowing = m_solutionParams->continuous_flowing_sol5;
		release_time = m_solutionParams->pulse_duration_well5;
		break;
	}
	case 6: {
		_bar = ui->progressBar_solution6;
		_button = ui->pushButton_solution6;
		status = m_pipette_status->rem_vol_well6;
		continuous_flowing = m_solutionParams->continuous_flowing_sol6;
		release_time = m_solutionParams->pulse_duration_well6;
		break;
	}

	default: {
		std::cerr << HERE 
			<< " error --- no valid m_flowing_solution value " << std::endl;
		m_update_flowing_sliders->stop();  // stop the timer to make sure the function is not recalled if the solutions are not flowing
		return;
	}
	}
	
	if (m_timer_solution < m_time_multipilcator) {
		m_update_flowing_sliders->start();
		//int status = int(100 * m_timer_solution / m_time_multipilcator);
		//_bar->setValue(100 - status); 
		QString s;
		if (!continuous_flowing) {
			s.append(m_str_pulse_remaining_time + "\n");
			//s.append(QString::number(m_flowing_solution));
			//s.append(" empty in \n");
			int remaining_time_in_sec = (m_time_multipilcator - m_timer_solution);
			int remaining_hours = floor(remaining_time_in_sec / 3600); // 3600 sec in a hour
			int remaining_mins = floor((remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
			int remaining_secs = remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
			s.append(QString::number(remaining_hours));
			s.append(" h ");
			s.append(QString::number(remaining_mins));
			s.append(" min ");
			s.append(QString::number(remaining_secs));
			s.append(" sec ");
			ui->textEdit_emptyTime->show();
			ui->textEdit_emptyTime->setText(s);
			m_timer_solution++;

			if (m_pipette_active) updateDrawing(m_ppc1->getZoneSizePerc());
			else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

			// show the warning label
			if (status < MIN_WARNING_VOLUME) {  
				ui->label_warningIcon->show();
				ui->label_warning->setText(m_str_warning_solution_end);
				ui->label_warning->show();
			}
			return;
		}
		else
		{
			s.append(m_str_pulse_continuous_flowing);  
			ui->textEdit_emptyTime->show();
			ui->textEdit_emptyTime->setText(s);

			// show the warning label
			if (status < MIN_WARNING_VOLUME) {  
				ui->label_warningIcon->show();
				ui->label_warning->setText(m_str_warning_solution_end);
				ui->label_warning->show();
			}
			return;
		}
		

		
	}
	else  // here we are ending the release process of the solution
	{
		double solution_release_time = release_time; // m_dialog_tools->getSolutionTime();
		m_time_multipilcator = (int)solution_release_time;
		double rest = solution_release_time - m_time_multipilcator;
		QThread::msleep(rest * 1000);
		// TODO: here we wait the remaining time for the last digit
		//       however, this is a shitty solution and it must be
		//       changed to a proper timer and interrupt architecture

		m_update_flowing_sliders->stop();
		m_timer_solution = 0;
		//_bar->setValue(10); //set the minimum just to visualize something
							//ui->widget_sol1->setValue(100);
		if (m_pipette_active)
		{
			//m_ppc1->setValve_l(false);
			//m_ppc1->closeAllValves(); //automatic shutdown of pumps when the solution ends
		}
		setEnableSolutionButtons(true);
		_button->setChecked(false);
		ui->widget_solutionArrow->setVisible(false);
		
		m_pen_line.setColor(Qt::transparent);
		updateDrawing(ui->lcdNumber_dropletSize_percentage->value());// (-1); // remove the droplet from the drawing

		ui->label_warningIcon->hide();
		ui->label_warning->hide();
		ui->textEdit_emptyTime->hide();// setText(" ");
		return;
	}

}