/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"


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
}

void Labonatip_GUI::pushSolution1()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pushSolution1    " << endl;

	if (!ui->pushButton_solution1->isChecked()) { // this allows to stop the flow when active
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		//m_update_time_s1->stop();
		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->pushButton_stop->setEnabled(false);
		ui->textEdit_emptyTime->setText(" ");
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

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol1_color);
	m_flowing_solution = 1;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

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
	ui->pushButton_stop->setEnabled(true);

	//	setEnableSolutionButtons(false);
	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution2() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pushSolution2   " << endl;

	if (!ui->pushButton_solution2->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->pushButton_stop->setEnabled(false);
		ui->textEdit_emptyTime->setText(" ");
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

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol2_color);
	m_flowing_solution = 2;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

	// move the arrow in the drawing to point on the solution 2
	ui->widget_solutionArrow->setVisible(true);
	ui->label_arrowSolution->setText(m_solutionParams->sol2);

	int pos_x = ui->progressBar_solution2->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution2->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

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
	ui->pushButton_stop->setEnabled(true);

	//setEnableSolutionButtons(false);
	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution3() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pushSolution3   " << endl;


	if (!ui->pushButton_solution3->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->pushButton_stop->setEnabled(false);
		ui->textEdit_emptyTime->setText(" ");
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

	// set the color into the drawing to fit the solution flow 
	m_pen_line.setColor(m_sol3_color);
	m_flowing_solution = 3;

	//if (m_pipette_active) updateDrawing(m_ppc1->getDropletSizePercentage());
	//else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

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
	ui->pushButton_stop->setEnabled(true);

	//setEnableSolutionButtons(false);
	updateFlows();
	updateFlowControlPercentages();

}

void Labonatip_GUI::pushSolution4() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pushSolution4   " << endl;


	if (!ui->pushButton_solution4->isChecked()) {
		m_timer_solution = m_time_multipilcator;

		if (m_pipette_active) {
			m_ppc1->closeAllValves();
		}

		m_update_flowing_sliders->stop();
		ui->widget_solutionArrow->setVisible(false);
		//updateDrawing(-1);
		m_pen_line.setColor(Qt::transparent);
		ui->pushButton_stop->setEnabled(false);
		ui->textEdit_emptyTime->setText(" ");
		updateFlowControlPercentages();
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
	ui->label_arrowSolution->setText(m_solutionParams->sol4);

	// calculate the middle point between the two widget to align the arrow to the progressbar
	int pos_x = ui->progressBar_solution4->pos().x() -
		ui->widget_solutionArrow->width() / 2 +
		ui->progressBar_solution4->width() / 2;
	ui->widget_solutionArrow->move(
		QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

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
	ui->pushButton_stop->setEnabled(true);

	//setEnableSolutionButtons(false);
	updateFlows();
	updateFlowControlPercentages();

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



void Labonatip_GUI::updateTimingSliders()
{
	QProgressBar *_bar;
	QPushButton *_button;

	switch (m_flowing_solution)
	{
	case 1: { //TODO : the waste time is not well calculated 
		_bar = ui->progressBar_solution1;
		_button = ui->pushButton_solution1;
		break;
	}
	case 2: {
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
	default: {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_GUI::updateTimingSliders  error --- no valid m_flowing_solution value " << endl;
		m_update_flowing_sliders->stop();  // stop the timer to make sure the function is not recalled if the solutions are not flowing
		return;
	}
	}
	
	if (m_timer_solution < m_time_multipilcator) {
		m_update_flowing_sliders->start();
		int status = int(100 * m_timer_solution / m_time_multipilcator);
		_bar->setValue(100 - status);
		QString s;
		if (!m_dialog_tools->isContinuousFlowing()) {
			s.append("Well ");
			s.append(QString::number(m_flowing_solution));
			s.append(" empty in \n");
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
		else
		{
			s.append("Well ");
			s.append(QString::number(m_flowing_solution));
			s.append(" in \n");
			s.append("continuous \nflowing");
		}
		ui->textEdit_emptyTime->setText(s);

		m_timer_solution++;

		if (m_pipette_active) updateDrawing(m_ppc1->getDropletSize());
		else updateDrawing(ui->lcdNumber_dropletSize_percentage->value());

		// show the warning label
		if (status > 50) {
			ui->label_warningIcon->show();
			ui->label_warning->show();
		}
		return;
	}
	else  // here we are ending the release process of the solution
	{
		/*if (m_dialog_tools->isContinuousFlowing())//(ui->checkBox_disableTimer->isChecked() ) // TODO: bring the param to settings
		{
			m_update_flowing_sliders->start();
			QString s;
			s.append("Well ");
			s.append(QString::number(m_flowing_solution));
			s.append(" in \n");
			s.append("continuous \nflowing");
			ui->textEdit_emptyTime->setText(s);
			return;
		}*/

		double solution_release_time = m_dialog_tools->getSolutionTime();
		m_time_multipilcator = (int)solution_release_time;
		double rest = solution_release_time - m_time_multipilcator;
		QThread::msleep(rest * 1000);
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

		ui->label_warningIcon->hide();
		ui->label_warning->hide();
		ui->pushButton_stop->setEnabled(false);
		ui->textEdit_emptyTime->setText(" ");
		return;
	}

}