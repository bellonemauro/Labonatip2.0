/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip.h"

#ifdef VLD_MEMORY_CHECK
  #include <vld.h>
#endif


void Labonatip_GUI::updateGUI() {

	//if (isExceptionTriggered()) return;

	// all the following update functions are for GUI only, 
	// there is no time calculation
	// on the contrary, waste update has specific timing 
	updatePressureVacuum();
	updateSolutions();
	updatePPC1Leds();
	updateFlows();

	if (m_ppc1->isRunning()) {
		//m_update_GUI->start(); // TODO: check, this should be running anyway
	}

	if (m_pipette_active) { 
		updateDrawing(m_ppc1->getZoneSizePerc()); 
	}
	else {
		updateDrawing(ui->lcdNumber_dropletSize_percentage->value());
	}
}

void Labonatip_GUI::updatePressureVacuum()
{
	if (!m_simulationOnly) {

		// if we are here it means that we have no exception 

		// for all pressures/vacuum, 
		// get the sensor reading ( rounded to second decimal?)
		int sensor_reading = (int)(m_ppc1->getVswitchReading());
		// update status, label and bar
		m_pipette_status->v_switch_set_point = -m_ppc1->getVswitchSetPoint();
		ui->label_switchPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(-m_pipette_status->v_switch_set_point)) + " mbar    "));
		ui->progressBar_switch->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->getVrecircReading());
		m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
		ui->label_recircPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(-m_pipette_status->v_recirc_set_point)) + " mbar    "));
		ui->progressBar_recirc->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->getPoffReading());
		m_pipette_status->poff_set_point = m_ppc1->getPoffSetPoint();
		ui->label_PoffPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(m_pipette_status->poff_set_point)) + " mbar    "));
		ui->progressBar_pressure_p_off->setValue(sensor_reading);

		sensor_reading = (int)(m_ppc1->getPonReading());
		m_pipette_status->pon_set_point = m_ppc1->getPonSetPoint();
		ui->label_PonPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(m_pipette_status->pon_set_point)) + " mbar    "));
		ui->progressBar_pressure_p_on->setValue(sensor_reading);

		// update droplet zone control zone size, flow speed and vacuum
		m_ds_perc = m_ppc1->getZoneSizePerc();
		m_fs_perc = m_ppc1->getFlowSpeedPerc();
		m_v_perc = m_ppc1->getVacuumPerc();

		// show the percentage in the display only if in the range [0,1000]
		if (m_ds_perc >= 0 && m_ds_perc < 1000) {
			ui->lcdNumber_dropletSize_percentage->display(m_ds_perc);
		}
		else { //otherwise it shows display error = "E"
			ui->lcdNumber_dropletSize_percentage->display(display_e);
		}

		if (m_fs_perc >= 0 && m_fs_perc < 1000) {
			ui->lcdNumber_flowspeed_percentage->display(m_fs_perc);
		}
		else {
			ui->lcdNumber_flowspeed_percentage->display(display_e);
		}

		if (m_v_perc >= 0 && m_v_perc < 1000) {
			ui->lcdNumber_vacuum_percentage->display(m_v_perc);
		}
		else {
			ui->lcdNumber_vacuum_percentage->display(display_e);
		}

		if (m_pipette_status->v_switch_set_point == 0 &&
			m_pipette_status->v_recirc_set_point == 0 &&
			m_pipette_status->pon_set_point == 0 &&
			m_pipette_status->poff_set_point == 0)
		{
			ui->lcdNumber_dropletSize_percentage->display(0);
			ui->lcdNumber_flowspeed_percentage->display(0);
			ui->lcdNumber_vacuum_percentage->display(0);
		}


	}// end if m_simulation
}

void Labonatip_GUI::updateSolutions()
{
	if (!m_simulationOnly) {
		// check for open wells 
		if (m_ppc1->isWeel1Open())
		{
			m_pen_line.setColor(m_sol1_color);

			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol1);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution1->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution1->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 1
			ui->pushButton_solution1->blockSignals(true);
			ui->pushButton_solution1->setChecked(true);
			ui->pushButton_solution1->blockSignals(false);

			// all the other buttons have to be off
			ui->pushButton_solution2->blockSignals(true);
			ui->pushButton_solution2->setChecked(false);
			ui->pushButton_solution2->blockSignals(false);

			ui->pushButton_solution3->blockSignals(true);
			ui->pushButton_solution3->setChecked(false);
			ui->pushButton_solution3->blockSignals(false);

			ui->pushButton_solution4->blockSignals(true);
			ui->pushButton_solution4->setChecked(false);
			ui->pushButton_solution4->blockSignals(false);

		}
		if (m_ppc1->isWeel2Open()) {
			m_pen_line.setColor(m_sol2_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol2);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution2->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution2->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 2
			ui->pushButton_solution2->blockSignals(true);
			ui->pushButton_solution2->setChecked(true);
			ui->pushButton_solution2->blockSignals(false);

			// all the other buttons have to be off
			ui->pushButton_solution1->blockSignals(true);
			ui->pushButton_solution1->setChecked(false);
			ui->pushButton_solution1->blockSignals(false);

			ui->pushButton_solution3->blockSignals(true);
			ui->pushButton_solution3->setChecked(false);
			ui->pushButton_solution3->blockSignals(false);

			ui->pushButton_solution4->blockSignals(true);
			ui->pushButton_solution4->setChecked(false);
			ui->pushButton_solution4->blockSignals(false);
		}
		if (m_ppc1->isWeel3Open()) {
			m_pen_line.setColor(m_sol3_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol3);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution3->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution3->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 3
			ui->pushButton_solution3->blockSignals(true);
			ui->pushButton_solution3->setChecked(true);
			ui->pushButton_solution3->blockSignals(false);

			// all the other buttons have to be off
			ui->pushButton_solution1->blockSignals(true);
			ui->pushButton_solution1->setChecked(false);
			ui->pushButton_solution1->blockSignals(false);

			ui->pushButton_solution2->blockSignals(true);
			ui->pushButton_solution2->setChecked(false);
			ui->pushButton_solution2->blockSignals(false);

			ui->pushButton_solution4->blockSignals(true);
			ui->pushButton_solution4->setChecked(false);
			ui->pushButton_solution4->blockSignals(false);
		}
		if (m_ppc1->isWeel4Open()) {
			m_pen_line.setColor(m_sol4_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol4);

			// calculate the middle point between the two widgets
			// to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution4->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution4->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));

			// switch on the button for the solution 4
			ui->pushButton_solution4->blockSignals(true);
			ui->pushButton_solution4->setChecked(true);
			ui->pushButton_solution4->blockSignals(false);

			// all the other buttons have to be off
			ui->pushButton_solution1->blockSignals(true);
			ui->pushButton_solution1->setChecked(false);
			ui->pushButton_solution1->blockSignals(false);

			ui->pushButton_solution2->blockSignals(true);
			ui->pushButton_solution2->setChecked(false);
			ui->pushButton_solution2->blockSignals(false);

			ui->pushButton_solution3->blockSignals(true);
			ui->pushButton_solution3->setChecked(false);
			ui->pushButton_solution3->blockSignals(false);
		}

		// if we have no open wells the droplet is removed from the drawing
		if (!m_ppc1->isWeel1Open() && !m_ppc1->isWeel2Open() &&
			!m_ppc1->isWeel3Open() && !m_ppc1->isWeel4Open()) {
			m_pen_line.setColor(Qt::transparent);
			ui->widget_solutionArrow->setVisible(false);

			// this connect the solution buttons to what happens in the PPC1
			// during the protocol running
			ui->pushButton_solution1->blockSignals(true);
			ui->pushButton_solution1->setChecked(false);
			ui->pushButton_solution1->blockSignals(false);

			ui->pushButton_solution2->blockSignals(true);
			ui->pushButton_solution2->setChecked(false);
			ui->pushButton_solution2->blockSignals(false);

			ui->pushButton_solution3->blockSignals(true);
			ui->pushButton_solution3->setChecked(false);
			ui->pushButton_solution3->blockSignals(false);

			ui->pushButton_solution4->blockSignals(true);
			ui->pushButton_solution4->setChecked(false);
			ui->pushButton_solution4->blockSignals(false);
		}

		// this should never happen, but the droplet would be removed anyway
		if (m_ppc1->isWeel1Open() && m_ppc1->isWeel2Open() &&
			m_ppc1->isWeel3Open() && m_ppc1->isWeel4Open()) {
			m_pen_line.setColor(Qt::transparent);
			ui->widget_solutionArrow->setVisible(false);

			// this connect the solution buttons to what happens in the PPC1
			// during the protocol running
			ui->pushButton_solution1->blockSignals(true);
			ui->pushButton_solution1->setChecked(false);
			ui->pushButton_solution1->blockSignals(false);

			ui->pushButton_solution2->blockSignals(true);
			ui->pushButton_solution2->setChecked(false);
			ui->pushButton_solution2->blockSignals(false);

			ui->pushButton_solution3->blockSignals(true);
			ui->pushButton_solution3->setChecked(false);
			ui->pushButton_solution3->blockSignals(false);

			ui->pushButton_solution4->blockSignals(true);
			ui->pushButton_solution4->setChecked(false);
			ui->pushButton_solution4->blockSignals(false);

		}

	}
}

void Labonatip_GUI::handlePPC1exception()
{

	// stop updating the GUI
	m_update_GUI->stop();

	// go back to the simulation mode
	ui->actionConnectDisconnect->setEnabled(false);
	ui->actionConnectDisconnect->setChecked(false);
	ui->actionConnectDisconnect->setText(m_str_connect);
	ui->actionReboot->setEnabled(false);
	ui->actionShudown->setEnabled(false);

	// disconnect from the PPC1
	m_ppc1->disconnectCOM();
	m_ppc1->stop();
	QThread::msleep(500);

	// verify that we are actually disconnected
	if (m_ppc1->isConnected())
		m_ppc1->disconnectCOM();
	QThread::msleep(500);

	// remember that the pipette is now deactivated
	m_pipette_active = false;

	// enable the simulation buttons
	ui->actionSimulation->setEnabled(true);
	ui->actionSimulation->setChecked(true);

	//this->setStatusLed(false);
	ui->status_PPC1_led->setPixmap(*led_red);
	ui->status_PPC1_label->setText(m_str_PPC1_status_discon);

	return;

	// if there is an exception, a message is out
	QMessageBox::information(NULL, m_str_warning,
		m_str_lost_connection + "<br>" + m_str_swapping_to_simulation);

}

bool Labonatip_GUI::isExceptionTriggered() // return true if the exception is triggered
{
	if (!m_simulationOnly) {
		// check exceptions, TODO: this is not the best way to do it !!!
		//if (m_ppc1->isConnected() && m_ppc1->isExceptionHappened()) { // this was there before, why ? the exception can happen connected or not
		if (m_ppc1->isExceptionHappened()) {
			//handlePPC1exception();
			// if there is an exception, a message is out
			//QMessageBox::information(this, m_str_warning,
			//	m_str_lost_connection + "<br>" + m_str_swapping_to_simulation);
			// end
			return true;
		}
	}
	return false;
}

void Labonatip_GUI::updatePPC1Leds()
{

	// update leds according to the PPC1 pressure/vacuum status, 
	// for each channel the state of the PPC1 is connected to the led
	// ok = 0 = green, 
	// fault = 1 = red
	// ok = 0 with reading far from the set point 10%+-3mbar = orange
	if (!m_simulationOnly) {

		if (m_ppc1->isRunning())
		{
			// the first is the communication state in the main GUI
			if (m_ppc1->getCommunicationState() == true) {
				//this->setStatusLed(true);
				ui->status_PPC1_led->setPixmap(*led_green);
				// It is better to avoid to change the LED directly, 
				// better to use the function setStatusLed(true/false)
				ui->status_PPC1_label->setText(m_str_PPC1_status_con);
			}
			else
			{
				//setStatusLed(false);
				ui->status_PPC1_led->setPixmap(*led_red);
				ui->status_PPC1_label->setText(m_str_PPC1_status_unstable_con); 
			}

			// update LED for Pon
			if (m_ppc1->getPonState() == 0) {
				ui->label_led_pon->setPixmap(*led_green);
				if (std::abs(m_ppc1->getPonReading() - m_ppc1->getPonSetPoint()) >
					0.1 * m_ppc1->getPonSetPoint() + 3)
				{
					ui->label_led_pon->setPixmap(*led_orange);
				}
			}
			else {
				ui->label_led_pon->setPixmap(*led_red);
			}

			// update LED for Poff
			if (m_ppc1->getPoffState() == 0) {
				ui->label_led_poff->setPixmap(*led_green);
				if (std::abs(m_ppc1->getPoffReading() - m_ppc1->getPoffSetPoint()) >
					0.1*m_ppc1->getPoffSetPoint() + 3)
				{
					ui->label_led_poff->setPixmap(*led_orange);
				}
			}
			else {
				ui->label_led_poff->setPixmap(*led_red);
			}

			// update LED for Vswitch
			if (m_ppc1->getVswitchState() == 0) {
				ui->label_led_vs->setPixmap(*led_green);
				if (std::abs(m_ppc1->getVswitchReading() - m_ppc1->getVswitchSetPoint()) >
					-0.1*m_ppc1->getVswitchSetPoint() + 3)
				{
					ui->label_led_vs->setPixmap(*led_orange);
				}
			}
			else {
				ui->label_led_vs->setPixmap(*led_red);
			}

			// update LED for Vrecirc
			if (m_ppc1->getVrecircState() == 0) {
				ui->label_led_vr->setPixmap(*led_green);
				double as = m_ppc1->getVrecircReading();
				double ad = m_ppc1->getVrecircSetPoint();
				if (std::abs(m_ppc1->getVrecircReading() - m_ppc1->getVrecircSetPoint()) >
					-0.1*m_ppc1->getVrecircSetPoint() + 3)
				{
					ui->label_led_vr->setPixmap(*led_orange);
				}
			}
			else {
				ui->label_led_vr->setPixmap(*led_red);
			}
		}// end if m_ppc1->isRunning()
		else
		{
			//this->setStatusLed(false);
			ui->status_PPC1_led->setPixmap(*led_red);
			ui->label_led_pon->setPixmap(*led_grey);
			ui->label_led_poff->setPixmap(*led_grey);
			ui->label_led_vs->setPixmap(*led_grey);
			ui->label_led_vr->setPixmap(*led_grey);

		}
	}//end if m_simulation_only
	else
	{
		ui->label_led_pon->setPixmap(*led_green);
		ui->label_led_poff->setPixmap(*led_green);
		ui->label_led_vs->setPixmap(*led_green);
		ui->label_led_vr->setPixmap(*led_green);

	}
}

void Labonatip_GUI::updateFlows()
{
	//shorter variables inside the function just for convenience
	double v_s = m_pipette_status->v_switch_set_point;
	double v_r = m_pipette_status->v_recirc_set_point;
	double p_on = m_pipette_status->pon_set_point;
	double p_off = m_pipette_status->poff_set_point;

	// if we are not in simulation, we just get the numbers from the PPC1api
	// otherwise the flows will be calculated according to the current values
	if (!m_simulationOnly)
	{ 
		fluicell::PPC1api::PPC1_status status = m_ppc1->getPipetteStatus();
		m_pipette_status->outflow_on = status.outflow_on;
		m_pipette_status->outflow_off = status.outflow_off;
		m_pipette_status->outflow_tot = status.outflow_tot;
		m_pipette_status->inflow_recirculation = status.inflow_recirculation;
		m_pipette_status->inflow_switch = status.inflow_switch;
		m_pipette_status->in_out_ratio_on = status.in_out_ratio_on;
		m_pipette_status->in_out_ratio_off = status.in_out_ratio_off;
		m_pipette_status->in_out_ratio_tot = status.in_out_ratio_tot;
		m_pipette_status->flow_well1 = status.flow_rate_1;
		m_pipette_status->flow_well2 = status.flow_rate_2;
		m_pipette_status->flow_well3 = status.flow_rate_3;
		m_pipette_status->flow_well4 = status.flow_rate_4;
		m_pipette_status->flow_well5 = status.flow_rate_5;
		m_pipette_status->flow_well6 = status.flow_rate_6;
		m_pipette_status->flow_well7 = status.flow_rate_7;
		m_pipette_status->flow_well8 = status.flow_rate_8;
	}
	else {
		// calculate inflow
		m_pipette_status->delta_pressure = 100.0 * v_r;
		m_pipette_status->inflow_recirculation = 
			2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLegthToTip());

		m_pipette_status->delta_pressure = 100.0 * (v_r + 
			2.0 * p_off * (1 - m_ppc1->getLegthToZone() / m_ppc1->getLegthToTip()));
		m_pipette_status->inflow_switch = 
			2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLegthToTip());

		m_pipette_status->delta_pressure = 100.0 * 2.0 * p_off;
		m_pipette_status->solution_usage_off = 
			m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * m_ppc1->getLegthToZone());

		m_pipette_status->delta_pressure = 100.0 * p_on;
		m_pipette_status->solution_usage_on = 
			m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLegthToTip());

		m_pipette_status->delta_pressure = 100.0 * (p_on + p_off * 3.0 - v_s * 2.0);   // TODO magic numbers
		m_pipette_status->outflow_on = 
			m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, m_ppc1->getLegthToTip());

		m_pipette_status->delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);
		m_pipette_status->outflow_off = 2.0 * m_ppc1->getFlowSimple(
			m_pipette_status->delta_pressure, 2 * m_ppc1->getLegthToZone());

		// this is to avoid NAN on the ratio
        if (m_pipette_status->inflow_recirculation == 0) 
			m_pipette_status->in_out_ratio_on = 0;
		else
			m_pipette_status->in_out_ratio_on =
			m_pipette_status->outflow_on / m_pipette_status->inflow_recirculation;

		// this is to avoid NAN on the ratio
        if (m_pipette_status->inflow_recirculation == 0)
			m_pipette_status->in_out_ratio_off = 0;
		else
			m_pipette_status->in_out_ratio_off =
			m_pipette_status->outflow_off / m_pipette_status->inflow_recirculation;

		// flow when solution is off 
		if (ui->pushButton_solution1->isChecked() ||
			ui->pushButton_solution2->isChecked() ||
			ui->pushButton_solution3->isChecked() ||
			ui->pushButton_solution4->isChecked()) 
		{
		
			m_pipette_status->in_out_ratio_tot = m_pipette_status->in_out_ratio_on;
			m_pipette_status->outflow_tot = m_pipette_status->outflow_on;

			if (ui->pushButton_solution1->isChecked()) 
				m_pipette_status->flow_well1 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well1 = m_pipette_status->solution_usage_off;

			if (ui->pushButton_solution2->isChecked()) 
				m_pipette_status->flow_well2 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well2 = m_pipette_status->solution_usage_off;

			if (ui->pushButton_solution3->isChecked())
				m_pipette_status->flow_well3 = m_pipette_status->solution_usage_on;
			else
				m_pipette_status->flow_well3 = m_pipette_status->solution_usage_off;

			if (ui->pushButton_solution4->isChecked()) 
				m_pipette_status->flow_well4 = m_pipette_status->solution_usage_on;
			else 
				m_pipette_status->flow_well4 = m_pipette_status->solution_usage_off;

		}
		else // flow when solution is on 
		{
			//m_pipette_status->delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);
			//m_pipette_status->outflow = 2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * LENGTH_TO_ZONE);

			m_pipette_status->in_out_ratio_tot = m_pipette_status->in_out_ratio_off;
			m_pipette_status->outflow_tot = m_pipette_status->outflow_off;

			m_pipette_status->flow_well1 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well2 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well3 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well4 = m_pipette_status->solution_usage_off;

		}
		m_pipette_status->flow_well5 = m_pipette_status->inflow_switch / 2.0;
		m_pipette_status->flow_well6 = m_pipette_status->inflow_switch / 2.0;
		m_pipette_status->flow_well7 = m_pipette_status->inflow_recirculation / 2.0;
		m_pipette_status->flow_well8 = m_pipette_status->inflow_recirculation / 2.0;
	}

	// update the tree widget
	ui->treeWidget_macroInfo->topLevelItem(0)->setText(1,
		QString::number(m_pipette_status->outflow_tot, 'g', 3));
	ui->treeWidget_macroInfo->topLevelItem(1)->setText(1,
		QString::number(m_pipette_status->inflow_recirculation, 'g', 4));
	ui->treeWidget_macroInfo->topLevelItem(2)->setText(1,
		QString::number(m_pipette_status->inflow_switch, 'g', 4));
	ui->treeWidget_macroInfo->topLevelItem(3)->setText(1,
		QString::number(m_pipette_status->in_out_ratio_tot, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(4)->setText(1,
		QString::number(m_pipette_status->flow_well1, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(5)->setText(1,
		QString::number(m_pipette_status->flow_well2, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(6)->setText(1,
		QString::number(m_pipette_status->flow_well3, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(7)->setText(1,
		QString::number(m_pipette_status->flow_well4, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(8)->setText(1,
		QString::number(m_pipette_status->flow_well5, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(9)->setText(1,
		QString::number(m_pipette_status->flow_well6, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(10)->setText(1,
		QString::number(m_pipette_status->flow_well7, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(11)->setText(1,
		QString::number(m_pipette_status->flow_well8, 'g', 2));

	return;
}



void Labonatip_GUI::updateDrawing(int _value) {


	if (_value <= 0) { // _value = -1 cleans the scene and make the flow disappear 

		m_scene_solution->clear();
		ui->graphicsView->update();
		ui->graphicsView->show();
		return;
	}

	if (_value >= MAX_ZONE_SIZE_PERC) {
		_value = MAX_ZONE_SIZE_PERC;
	}

	//clean the scene
	m_scene_solution->clear();

	// draw the circle 
	QBrush brush(m_pen_line.color(), Qt::SolidPattern);

	QPen border_pen;
	border_pen.setColor(Qt::transparent);
	border_pen.setWidth(1);

	double droplet_modifier = (10.0 - _value / 10.0);

	// this is an attempt to make the droplet to look a little bit more realistic
	QPainterPath droplet;
	droplet.arcMoveTo((qreal)ui->doubleSpinBox_d_x->value() + droplet_modifier, (qreal)ui->doubleSpinBox_d_y->value(),
		(qreal)ui->doubleSpinBox_d_w->value() - droplet_modifier, (qreal)ui->doubleSpinBox_d_h->value(), (qreal)ui->doubleSpinBox_d_a->value());
	
	droplet.arcTo((qreal)ui->doubleSpinBox_d2x->value() + droplet_modifier, (qreal)ui->doubleSpinBox_d2y->value(),
		(qreal)ui->doubleSpinBox_d2w->value() - droplet_modifier, (qreal)ui->doubleSpinBox_d2h->value(),
		(qreal)ui->doubleSpinBox_d2a->value(), (qreal)ui->doubleSpinBox_d2l->value());


	droplet.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(droplet, border_pen, brush);

	//all this function is rather GUI fix stuff and the numbers can be set using an hidden visualization tool
	QPainterPath circle;
	circle.arcMoveTo((qreal)ui->doubleSpinBox_c_x->value(), (qreal)ui->doubleSpinBox_c_y->value(),
		(qreal)ui->doubleSpinBox_c_w->value(), (qreal)ui->doubleSpinBox_c_h->value(),
		(qreal)ui->doubleSpinBox_c_a->value());  

	circle.arcTo((qreal)ui->doubleSpinBox_c2x->value(), (qreal)ui->doubleSpinBox_c2y->value(),
		(qreal)ui->doubleSpinBox_c2w->value(), (qreal)ui->doubleSpinBox_c2h->value(),
		(qreal)ui->doubleSpinBox_c2a->value(), (qreal)ui->doubleSpinBox_c2l->value());

	circle.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(circle, border_pen, brush);

	int border_pen_pipe_width = 7; 
	QBrush brush_pipes(Qt::transparent, Qt::NoBrush);
	QPen border_pen_pipe1;
	border_pen_pipe1.setColor(m_sol3_color); 
	border_pen_pipe1.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe1;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);

	path_pipe1.arcMoveTo((qreal)ui->doubleSpinBox_p1_x->value(), (qreal)ui->doubleSpinBox_p1_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p1_w->value(), (qreal)ui->doubleSpinBox_p1_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p1_a->value());

	path_pipe1.arcTo((qreal)ui->doubleSpinBox_p12x->value(), (qreal)ui->doubleSpinBox_p12y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p12w->value(), (qreal)ui->doubleSpinBox_p12h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p12a->value(), (qreal)ui->doubleSpinBox_p12l->value()); //qreal startAngle, qreal arcLength


	path_pipe1.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe1, border_pen_pipe1, brush_pipes);

	QPen border_pen_pipe2;
	border_pen_pipe2.setColor(m_sol1_color); 
	border_pen_pipe2.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe2;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);


	path_pipe2.arcMoveTo((qreal)ui->doubleSpinBox_p2_x->value(), (qreal)ui->doubleSpinBox_p2_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p2_w->value(), (qreal)ui->doubleSpinBox_p2_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p2_a->value());
	path_pipe2.arcTo((qreal)ui->doubleSpinBox_p22x->value(), (qreal)ui->doubleSpinBox_p22y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p22w->value(), (qreal)ui->doubleSpinBox_p22h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p22a->value(), (qreal)ui->doubleSpinBox_p22l->value()); //qreal startAngle, qreal arcLength

	path_pipe2.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe2, border_pen_pipe2, brush_pipes);

	
	QPen border_pen_pipe3;
	border_pen_pipe3.setColor(m_sol2_color); 
	border_pen_pipe3.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe3; 
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe3.arcMoveTo((qreal)ui->doubleSpinBox_p3_x->value(), (qreal)ui->doubleSpinBox_p3_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p3_w->value(), (qreal)ui->doubleSpinBox_p3_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p3_a->value());
	path_pipe3.arcTo((qreal)ui->doubleSpinBox_p32x->value(), (qreal)ui->doubleSpinBox_p32y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p32w->value(), (qreal)ui->doubleSpinBox_p32h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p32a->value(), (qreal)ui->doubleSpinBox_p32l->value()); //qreal startAngle, qreal arcLength
	path_pipe3.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe3, border_pen_pipe3, brush_pipes);

	
	QPen border_pen_pipe4;
	border_pen_pipe4.setColor(m_sol4_color);
	border_pen_pipe4.setWidth(border_pen_pipe_width);
	QPainterPath path_pipe4;
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe4.arcMoveTo((qreal)ui->doubleSpinBox_p4_x->value(), (qreal)ui->doubleSpinBox_p4_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p4_w->value(), (qreal)ui->doubleSpinBox_p4_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p4_a->value());

	path_pipe4.arcTo((qreal)ui->doubleSpinBox_p42x->value(), (qreal)ui->doubleSpinBox_p42y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p42w->value(), (qreal)ui->doubleSpinBox_p42h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p42a->value(), (qreal)ui->doubleSpinBox_p42l->value()); //qreal startAngle, qreal arcLength

	path_pipe4.setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(path_pipe4, border_pen_pipe4, brush_pipes);

	// draw a line from the injector to the solution release point 
	m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);
	
	ui->graphicsView->setScene(m_scene_solution);
	ui->graphicsView->show();

	return;
}

// this is updated every second
// this does not work with PPC1api (only works in simulation)
void Labonatip_GUI::updateWaste() 
{
	// NB:  here there is a calculation of the volume as follows:
	//      remaining volume = current_volume (mL) - delta_t * flow_rate (nL/s)
	//      there is an assumption of this to run every second hence delta_t = 1  
	//      this should be changed to fit the real values of timers
	// when we switch from simulation to real device, wells should be reset ?
	//m_update_waste->start();

	if (m_ds_perc < 10) return;

	double waste_remaining_time_in_sec;

	//updateWells();
	int max = MAX_VOLUME_IN_WELL;  // just to have a shorter name in formulas

								   // update wells when the solution is flowing
	if (ui->pushButton_solution1->isChecked()) {
		m_pipette_status->rem_vol_well1 = m_pipette_status->rem_vol_well1 -
			0.001 * m_pipette_status->flow_well1; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well1 < 0) {
			stopSolutionFlow();
			QMessageBox::information(this, m_str_warning,
				m_str_solution_ended);

			// deprecated 06072018
			//QMessageBox::StandardButton resBtn =
			//	QMessageBox::question(this, m_str_warning,
			//		QString(m_str_solution_ended),
			//		QMessageBox::No | QMessageBox::Yes,
			//		QMessageBox::Yes);
			//if (resBtn == QMessageBox::Yes) {
			//	refillSolution();
			//}
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well1 / max;
		ui->progressBar_solution1->setValue(int(perc));
	}
	if (ui->pushButton_solution2->isChecked()) {
		m_pipette_status->rem_vol_well2 = m_pipette_status->rem_vol_well2 -
			0.001 * m_pipette_status->flow_well2; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well2 < 0) {
			stopSolutionFlow();
			QMessageBox::information(this, m_str_warning,
				m_str_solution_ended);
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well2 / max;
		ui->progressBar_solution2->setValue(int(perc));
	}
	if (ui->pushButton_solution3->isChecked()) {
		m_pipette_status->rem_vol_well3 = m_pipette_status->rem_vol_well3 -
			0.001 * m_pipette_status->flow_well3; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well3 < 0) {
			stopSolutionFlow();
			QMessageBox::information(this, m_str_warning,
				m_str_solution_ended);
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well3 / max;
		ui->progressBar_solution3->setValue(int(perc));
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_pipette_status->rem_vol_well4 = m_pipette_status->rem_vol_well4 -
			0.001 * m_pipette_status->flow_well4; // 0.001 is to transform in mL/s

		if (m_pipette_status->rem_vol_well4 < 0) {
			stopSolutionFlow();
			QMessageBox::information(this, m_str_warning,
				m_str_solution_ended);
		}

		double perc = 100.0 * m_pipette_status->rem_vol_well4 / max;
		ui->progressBar_solution4->setValue(int(perc));
	}


	m_pipette_status->rem_vol_well5 = m_pipette_status->rem_vol_well5 +
		0.001 * m_pipette_status->flow_well5;
	m_pipette_status->rem_vol_well6 = m_pipette_status->rem_vol_well6 +
		0.001 * m_pipette_status->flow_well6;
	m_pipette_status->rem_vol_well7 = m_pipette_status->rem_vol_well7 +
		0.001 * m_pipette_status->flow_well7;
	m_pipette_status->rem_vol_well8 = m_pipette_status->rem_vol_well8 +
		0.001 * m_pipette_status->flow_well8;

	// show the warning label
	if (m_pipette_status->rem_vol_well5 > MAX_WASTE_WARNING_VOLUME ||
		m_pipette_status->rem_vol_well6 > MAX_WASTE_WARNING_VOLUME ||
		m_pipette_status->rem_vol_well7 > MAX_WASTE_WARNING_VOLUME ||
		m_pipette_status->rem_vol_well8 > MAX_WASTE_WARNING_VOLUME) {
		ui->label_warningIcon->show();
		ui->label_warning->setText(m_str_warning_waste_full);
		ui->label_warning->show();
	}

	// only the minimum of the remaining solution is shown and important
	vector<double> v1;
	v1.push_back(m_solutionParams->vol_well5 - m_pipette_status->rem_vol_well5);
	v1.push_back(m_solutionParams->vol_well6 - m_pipette_status->rem_vol_well6);
	v1.push_back(m_solutionParams->vol_well7 - m_pipette_status->rem_vol_well7);
	v1.push_back(m_solutionParams->vol_well8 - m_pipette_status->rem_vol_well8);

	auto min = std::min_element(v1.begin(), v1.end());
	int min_index = std::distance(v1.begin(), min);


	switch (min_index)
	{
	case 0: {
		if (m_pipette_status->flow_well5 > 0) {  
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well5 -
				m_pipette_status->rem_vol_well5) / m_pipette_status->flow_well5;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 1: {
		if (m_pipette_status->flow_well6 > 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well6 -
				m_pipette_status->rem_vol_well6) / m_pipette_status->flow_well6;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 2: {
		if (m_pipette_status->flow_well7 > 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well7 -
				m_pipette_status->rem_vol_well7) / m_pipette_status->flow_well7;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 3: {
		if (m_pipette_status->flow_well8 > 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well8 -
				m_pipette_status->rem_vol_well8) / m_pipette_status->flow_well8;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	default: {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_GUI::updateWaste  error --- no max found " << endl;
	}
	}

	int v = m_pipette_status->rem_vol_well1 * 10; // this is to set a single decimal digit
	ui->treeWidget_macroInfo->topLevelItem(12)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well2 * 10;
	ui->treeWidget_macroInfo->topLevelItem(13)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well3 * 10;
	ui->treeWidget_macroInfo->topLevelItem(14)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well4 * 10;
	ui->treeWidget_macroInfo->topLevelItem(15)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well5 * 10;
	ui->treeWidget_macroInfo->topLevelItem(16)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well6 * 10;
	ui->treeWidget_macroInfo->topLevelItem(17)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well7 * 10;
	ui->treeWidget_macroInfo->topLevelItem(18)->setText(1, QString::number(v / 10.0));
	v = m_pipette_status->rem_vol_well8 * 10;
	ui->treeWidget_macroInfo->topLevelItem(19)->setText(1, QString::number(v / 10.0));

	double value = 100.0 *  m_pipette_status->rem_vol_well5 / m_solutionParams->vol_well5;
	ui->progressBar_switchOut->setValue(value);

	value = 100.0 * m_pipette_status->rem_vol_well6 / m_solutionParams->vol_well6;
	ui->progressBar_switchIn->setValue(value);

	value = 100.0 * m_pipette_status->rem_vol_well7 / m_solutionParams->vol_well7;
	ui->progressBar_recircOut->setValue(value);

	value = 100.0 * m_pipette_status->rem_vol_well8 / m_solutionParams->vol_well8;
	ui->progressBar_recircIn->setValue(value);


	if (waste_remaining_time_in_sec < 0) {

		waste_remaining_time_in_sec = 0; //this is to avoid to show negative values 

		// if we have 0 remaining time but the remainder is active, we keep going
		if (m_waste_remainder->isActive()) return;


		// otherwise, show a message and start a remainder timer
		QMessageBox::information(this, m_str_warning,
			m_str_waste_full);

		m_waste_remainder->start();

		// deprecated 07072018
		//QMessageBox::StandardButton resBtn =
		//	QMessageBox::question(this, m_str_warning,
		//		QString(m_str_waste_full),
		//		QMessageBox::No | QMessageBox::Yes,
		//		QMessageBox::Yes);
		//if (resBtn == QMessageBox::Yes) {
		//	emptyWells();
		//}

		return;
	}

	// build the string for the waste label
	QString s;
	s.append(m_str_pulse_waste + " ");
	s.append(QString::number(min_index + 5));
	s.append(" " + m_str_pulse_full_in + ": ");
	int remaining_hours = floor(waste_remaining_time_in_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor(((int)waste_remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = waste_remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	//s.append(" h ");
	//s.append(QString::number(remaining_mins));
	//s.append(" min ");
	s.append(" ");
	s.append(m_str_h);
	s.append(",   ");
	s.append(QString::number(remaining_mins));
	s.append(" ");
	s.append(m_str_min);
	//s.append(QString::number(remaining_secs));
	//s.append(" sec ");

	ui->textEdit_emptyTime_waste->setText(s);
}

void Labonatip_GUI::emptyWasteRemainder()
{

	QMessageBox::information(this, m_str_warning,
		m_str_waste_full);

	m_waste_remainder->start();
	
}


void Labonatip_GUI::updateMacroStatusMessage(const QString &_message) {

	// this is not used for now, the protocol runner sends a message here, 
	// but this is not currently passed to the GUI
    QString s = " PROTOCOL RUNNING : <<<  ";
	s.append(m_current_protocol_file_name);
    s.append(" >>> remaining time = "); 

    s.append(_message);

}


void Labonatip_GUI::updateMacroTimeStatus(const double &_status) 
{
	// update the vertical line for the time status on the chart
	m_chart_view->updateChartTime(_status);

    QString s = m_str_update_time_macro_msg1;
	QFileInfo fi(m_current_protocol_file_name);
	s.append(fi.fileName());
	
	s.append(m_str_update_time_macro_msg2);

	//int remaining_time_sec = m_protocol_duration - _status * m_protocol_duration / 100;
	double duration = m_ppc1->protocolDuration(*m_protocol);
	int remaining_time_sec = duration - _status * duration / 100;
    int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
    int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
    int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
    
	s.append(QString::number(remaining_hours));
    //s.append(" h,   ");
    //s.append(QString::number(remaining_mins));
    //s.append(" min,   ");
    //s.append(QString::number(remaining_secs));
    //s.append(" sec   ");
	s.append(" ");
	s.append(m_str_h);
	s.append(",   ");
	s.append(QString::number(remaining_mins));
	s.append(" ");
	s.append(m_str_min);
	s.append(",   ");
	s.append(QString::number(remaining_secs));
	s.append(" ");
	s.append(m_str_sec);
	s.append("   ");
    ui->progressBar_macroStatus->setValue(_status);
    ui->label_macroStatus->setText(s);

    s.clear();
    s.append(QString::number(m_macroRunner_thread->getTimeLeftForStep()));
    s.append(" s");
    ui->label_duration->setText(s);


    // update the slider for the GUI
    ui->horizontalSlider_recirculation->blockSignals(true);
    ui->horizontalSlider_recirculation->setValue(m_ppc1->getVrecircSetPoint());
    ui->horizontalSlider_recirculation->blockSignals(false);

    ui->horizontalSlider_switch->blockSignals(true);
    ui->horizontalSlider_switch->setValue(m_ppc1->getVswitchSetPoint());
    ui->horizontalSlider_switch->blockSignals(false);

    ui->horizontalSlider_p_off->blockSignals(true);
    ui->horizontalSlider_p_off->setValue(m_ppc1->getPoffSetPoint());
    ui->horizontalSlider_p_off->blockSignals(false);

    ui->horizontalSlider_p_on->blockSignals(true);
    ui->horizontalSlider_p_on->setValue(m_ppc1->getPonSetPoint());
    ui->horizontalSlider_p_on->blockSignals(false);

	double currentTime = _status * duration / 100.0 ;
    updateFlowControlPercentages();

}
