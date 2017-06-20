/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"



void Labonatip_GUI::dropletSizePlus() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::dropletSizePlus    " << endl;
	// only Pon + 2.5%
	// V_recirc - 2.5%
	if (m_pipette_active) {
		if (!m_ppc1->increaseDropletSize())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}

	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0) {
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
		else {
			ui->horizontalSlider_p_on->setValue(
				ui->horizontalSlider_p_on->value() * 1.025);
		}

		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::dropletSizeMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::dropletSizeMinus    " << endl;

	// only Pon - 2.5%
	// V_recirc + 2.5%
	if (m_pipette_active) {
		if (!m_ppc1->decreaseDropletSize())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}

	if (m_simulationOnly) {

		if (ui->horizontalSlider_p_on->value() == 0) {
			QMessageBox::information(this, "Warning !", " P_on zero, operation cannot be done ");
			return;
		}
		else {
			ui->horizontalSlider_p_on->setValue(
				ui->horizontalSlider_p_on->value() * 0.975);
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			ui->horizontalSlider_recirculation->setValue(3);
		}
		else {

			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 1.025);
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedPlus() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::flowSpeedPlus    " << endl;
	// +5% to all values
	// Poff does not read too low values, 
	// if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	if (m_pipette_active) {
		if (!m_ppc1->increaseFlowspeed())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_p_on->value() == 0) {
			ui->horizontalSlider_p_on->setValue(5);
		}
		else {
			ui->horizontalSlider_p_on->setValue(
				ui->horizontalSlider_p_on->value() * 1.05);
		}

		if (ui->horizontalSlider_p_off->value() == 0) {
			ui->horizontalSlider_p_off->setValue(5);
		}
		else {

			ui->horizontalSlider_p_off->setValue(
				ui->horizontalSlider_p_off->value() * 1.05);
		}

		if (ui->horizontalSlider_switch->value() == 0) {
			ui->horizontalSlider_switch->setValue(5);
		}
		else {

			ui->horizontalSlider_switch->setValue(
				ui->horizontalSlider_switch->value() * 1.05);
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			ui->horizontalSlider_recirculation->setValue(5);
		}
		else {

			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 1.05);
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedMinus() {


	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::flowSpeedMinus    " << endl;

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

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::vacuumPlus    " << endl;

	// +5% v_recirculation
	if (m_pipette_active) {
		if (!m_ppc1->increaseVacuum5p())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values.");
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0) {
			ui->horizontalSlider_recirculation->setValue(5);
		}
		else {

			ui->horizontalSlider_recirculation->setValue(
				ui->horizontalSlider_recirculation->value() * 1.05);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::vacuumMinus    " << endl;

	// -5% v_recirculation
	if (m_pipette_active) {
		if (!m_ppc1->decreaseVacuum5p())
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0) {
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
				ui->horizontalSlider_p_on->value() / 190;//
			m_dialog_tools->m_pr_params->p_on_default; // TODO direct access the public member, add get function instead

			int vr = 100 *
				ui->horizontalSlider_recirculation->value() /
				(-m_dialog_tools->m_pr_params->v_recirc_default);  // TODO direct access the public member, add get function instead

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
				(-m_dialog_tools->m_pr_params->v_switch_default); // TODO direct access the public member, add get function instead;

			int vr = 100 *
				ui->horizontalSlider_recirculation->value() /
				(-m_dialog_tools->m_pr_params->v_recirc_default); // TODO direct access the public member, add get function instead;

			int flowspeed_percentage = (pon + poff + vs + vr) / 4; // 4 is for the average
			ui->lcdNumber_flowspeed_percentage->display(flowspeed_percentage);
		}

		{
			int vacuum_percentage = 100 *
				ui->horizontalSlider_recirculation->value() /
				(-m_dialog_tools->m_pr_params->v_recirc_default); // TODO direct access the public member, add get function instead;

			ui->lcdNumber_vacuum_percentage->display(vacuum_percentage);
		}
	}

}



