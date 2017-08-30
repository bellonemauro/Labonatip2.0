/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
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
		if (!m_ppc1->increaseDropletSize()) {
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
		}
		else
		{
			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;
			m_pon_set_point = m_ppc1->m_PPC1_data->channel_D->set_point;

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
			ui->horizontalSlider_p_on->blockSignals(true);
			ui->horizontalSlider_p_on->setValue(m_pon_set_point);
			ui->horizontalSlider_p_on->blockSignals(false);
		}
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0) {
			QMessageBox::information(this, "Warning !", " Recirculation zero, operation cannot be done ");
			return;
		}
		else {
			double value = m_v_recirc_set_point - default_v_recirc * 0.025;
			updateVrecircSetPoint(value);
			cout << "Labonatip_GUI::dropletSizeMinus    ::: new recirculation value " << value << endl;
		}

		if (ui->horizontalSlider_p_on->value() == 0) {
			updatePonSetPoint(3.0);
		}
		else {
			double value = m_pon_set_point + default_pon * 0.025;
			updatePonSetPoint(value);
			cout << "Labonatip_GUI::dropletSizeMinus    ::: new pon value " << value << endl;
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
		if (!m_ppc1->decreaseDropletSize()) {
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
		}
		else
		{
			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;
			m_pon_set_point = m_ppc1->m_PPC1_data->channel_D->set_point;

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
			ui->horizontalSlider_p_on->blockSignals(true);
			ui->horizontalSlider_p_on->setValue(m_pon_set_point);
			ui->horizontalSlider_p_on->blockSignals(false);
		}
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_p_on->value() == 0) {
			QMessageBox::information(this, "Warning !", " P_on zero, operation cannot be done ");
			return;
		}
		else {
			double value = m_pon_set_point - default_pon * 0.025;
			updatePonSetPoint(value);
			cout << "Labonatip_GUI::dropletSizeMinus    ::: new pon value " << value << endl;
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			updateVrecircSetPoint(-3.0);
		}
		else {
			double value = m_v_recirc_set_point + default_v_recirc * 0.025;
			updateVrecircSetPoint(value);
			cout << "Labonatip_GUI::dropletSizeMinus    ::: new recirculation value " << value << endl;
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
		if (!m_ppc1->increaseFlowspeed()) {
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
		}
		else
		{
			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;
			m_v_switch_set_point = -m_ppc1->m_PPC1_data->channel_B->set_point;
			m_poff_set_point = m_ppc1->m_PPC1_data->channel_C->set_point;
			m_pon_set_point = m_ppc1->m_PPC1_data->channel_D->set_point;

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
			ui->horizontalSlider_switch->blockSignals(true);
			ui->horizontalSlider_switch->setValue(m_v_switch_set_point);
			ui->horizontalSlider_switch->blockSignals(false);
			ui->horizontalSlider_p_off->blockSignals(true);
			ui->horizontalSlider_p_off->setValue(m_poff_set_point);
			ui->horizontalSlider_p_off->blockSignals(false);
			ui->horizontalSlider_p_on->blockSignals(true);
			ui->horizontalSlider_p_on->setValue(m_pon_set_point);
			ui->horizontalSlider_p_on->blockSignals(false);
		}
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_p_on->value() == 0) {
			updatePonSetPoint(5.0);
		}
		else {
			double value = m_pon_set_point + default_pon * 0.05;
			updatePonSetPoint(value);
			cout << "Labonatip_GUI::flowSpeedPlus    ::: new pon value " << value << endl;
		}

		if (ui->horizontalSlider_p_off->value() == 0) {
			updatePoffSetPoint(5.0);
		}
		else {
			double value = m_poff_set_point + default_poff * 0.05;
			updatePoffSetPoint(value);
			cout << "Labonatip_GUI::flowSpeedPlus    ::: new poff value " << value << endl;
		}

		if (ui->horizontalSlider_switch->value() == 0) {
			updateVswitchSetPoint ( 5.0 );
		}
		else {
			double value = m_v_switch_set_point + default_v_switch * 0.05;
			updateVswitchSetPoint(value);
			cout << "Labonatip_GUI::flowSpeedPlus    ::: new switch value " << value << endl;
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			updateVrecircSetPoint(5.0);
		}
		else {
			double value = m_v_recirc_set_point + default_v_recirc * 0.05;
			updateVrecircSetPoint(value);
			cout << "Labonatip_GUI::flowSpeedPlus    ::: new recirculation value " << value << endl;
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedMinus() {


	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::flowSpeedMinus    " << endl;

	// -5% to all values
	if (m_pipette_active) {
		if (!m_ppc1->decreaseFlowspeed()){
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
		}
		else
		{

			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;
			m_v_switch_set_point = -m_ppc1->m_PPC1_data->channel_B->set_point;
			m_poff_set_point = m_ppc1->m_PPC1_data->channel_C->set_point;
			m_pon_set_point = m_ppc1->m_PPC1_data->channel_D->set_point;

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
			ui->horizontalSlider_switch->blockSignals(true);
			ui->horizontalSlider_switch->setValue(m_v_switch_set_point);
			ui->horizontalSlider_switch->blockSignals(false);
			ui->horizontalSlider_p_off->blockSignals(true);
			ui->horizontalSlider_p_off->setValue(m_poff_set_point);
			ui->horizontalSlider_p_off->blockSignals(false);
			ui->horizontalSlider_p_on->blockSignals(true);
			ui->horizontalSlider_p_on->setValue(m_pon_set_point);
			ui->horizontalSlider_p_on->blockSignals(false);
		}
	}
	if (m_simulationOnly) {

		double value = m_pon_set_point - default_pon * 0.05;
		updatePonSetPoint(value);
		cout << "Labonatip_GUI::flowSpeedMinus    ::: new pon value " << value << endl;
		value = m_poff_set_point - default_poff * 0.05;
		updatePoffSetPoint(value);
		cout << "Labonatip_GUI::flowSpeedMinus    ::: new poff value " << value << endl;
		value = m_v_switch_set_point - default_v_switch * 0.05;
		updateVswitchSetPoint(value);
		cout << "Labonatip_GUI::flowSpeedMinus    ::: new switch value " << value << endl;
		value = m_v_recirc_set_point - default_v_recirc * 0.05;
		updateVrecircSetPoint(value);
		cout << "Labonatip_GUI::flowSpeedMinus    ::: new recirculation value " << value << endl;

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumPlus() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::vacuumPlus    " << endl;

	// +5% v_recirculation
	if (m_pipette_active) {
		if (!m_ppc1->increaseVacuum()){
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values.");
		}
		else {
			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;
			//ui->label_recircPressure->setText(QString(QString::number(m_v_recirc_set_point) + " mbar"));

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
		}
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0) {
			ui->horizontalSlider_recirculation->setValue(5);
		}
		else {

			double value = m_v_recirc_set_point + default_v_recirc * 0.05;
			cout << "Labonatip_GUI::vacuumPlus    ::: new recirculation value " << value << endl;
			updateVrecircSetPoint(value);
			
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::vacuumMinus    " << endl;

	// -5% v_recirculation
	if (m_pipette_active) {
		if (!m_ppc1->decreaseVacuum()){
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Please, check for out of bound values. ");
		}
		else {
			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;
			//ui->label_recircPressure->setText(QString(QString::number(m_v_recirc_set_point) + " mbar"));

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);

		}
	}
	if (m_simulationOnly) {

		if (ui->horizontalSlider_recirculation->value() == 0) {
			QMessageBox::information(this, "Warning !", " Operation cannot be done. <br> Recirculation is zero. ");
		}

		double value = m_v_recirc_set_point - default_v_recirc * 0.05;
		cout << "Labonatip_GUI::vacuumMinus    ::: new recirculation value " << value << endl;
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::updateFlowControlPercentages()
{

	if (m_simulationOnly) {

		{
			double ponp = 100.0 * m_pon_set_point / default_pon;
			double vrp = 100.0 * m_v_recirc_set_point / default_v_recirc;

			double droplet_percentage = 100.0 + (ponp - vrp) / 2.0;
			ui->lcdNumber_dropletSize_percentage->display(droplet_percentage);
			//ui->progressBar_dropletSize->setValue(droplet_percentage);
		}
		{
			double ponp = 100.0 * m_pon_set_point / default_pon; 
			double poffp = 100.0 * m_poff_set_point / default_poff; 
			double vsp = 100.0 * m_v_switch_set_point / default_v_switch; 
			double vrp = 100.0 * m_v_recirc_set_point / default_v_recirc; 

			double flowspeed_percentage = (ponp + poffp + vsp + vrp) / 4.0; // 4 is for the average
			ui->lcdNumber_flowspeed_percentage->display(flowspeed_percentage);
			//ui->progressBar_flowSpeed->setValue(flowspeed_percentage);
		}
		{
			double vacuum_percentage = 100.0 * m_v_recirc_set_point / default_v_recirc; 
			ui->lcdNumber_vacuum_percentage->display(vacuum_percentage);
			//ui->progressBar_vacuum->setValue(vacuum_percentage);
		}
	}

	ui->progressBar_recircIn->setValue(ui->horizontalSlider_recirculation->value());
	ui->progressBar_recircOut->setValue(ui->horizontalSlider_recirculation->value());

	ui->progressBar_switchIn->setValue(ui->horizontalSlider_switch->value());
	ui->progressBar_switchOut->setValue(ui->horizontalSlider_switch->value());

	//updateDrawing(ui->progressBar_dropletSize->value());

}



