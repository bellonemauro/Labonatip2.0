/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"


void Labonatip_GUI::updatePonSetPoint(double _pon_set_point)
{
	// update the set point
	m_pon_set_point = _pon_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updatePonSetPoint   ::::  value  =  " << m_pon_set_point << endl;

	ui->label_PonPressure->setText(QString(QString::number(int(m_pon_set_point)) + " mbar"));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setPressureChannelD(m_pon_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_p_on->blockSignals(true);
	ui->horizontalSlider_p_on->setValue(m_pon_set_point);
	ui->horizontalSlider_p_on->blockSignals(false);

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_on->setValue(m_pon_set_point);
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::updatePoffSetPoint(double _poff_set_point)
{	
	// update the set point
	m_poff_set_point = _poff_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updatePoffSetPoint   ::::  value  =  " << m_poff_set_point << endl;

	ui->label_PoffPressure->setText(QString(QString::number(int(m_poff_set_point)) + " mbar"));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setPressureChannelC(m_poff_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_p_off->blockSignals(true);
	ui->horizontalSlider_p_off->setValue(m_poff_set_point);
	ui->horizontalSlider_p_off->blockSignals(false);

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_off->setValue(m_poff_set_point);
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::updateVrecircSetPoint(double _v_recirc_set_point)
{
	// update the set point
	m_v_recirc_set_point = _v_recirc_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateVrecircSetPoint   ::::  value  =  " << m_v_recirc_set_point << endl;
	ui->label_recircPressure->setText(QString(QString::number(-int(m_v_recirc_set_point)) + " mbar"));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setVacuumChannelA(-m_v_recirc_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_recirculation->blockSignals(true);
	ui->horizontalSlider_recirculation->setValue(m_v_recirc_set_point);
	ui->horizontalSlider_recirculation->blockSignals(false);

	if (m_simulationOnly) {

		ui->progressBar_recirc->setValue(m_v_recirc_set_point);
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::updateVswitchSetPoint(double _v_switch_set_point)
{
	// update the set point
	m_v_switch_set_point = _v_switch_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateVswitchSetPoint   ::::  value  =  " << m_v_switch_set_point << endl;
	ui->label_switchPressure->setText(QString(QString::number(-int(m_v_switch_set_point)) + " mbar"));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setVacuumChannelB(-m_v_switch_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_switch->blockSignals(true);
	ui->horizontalSlider_switch->setValue(m_v_switch_set_point);
	ui->horizontalSlider_switch->blockSignals(false);

	if (m_simulationOnly) {
		ui->progressBar_switch->setValue(m_v_switch_set_point);
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::pressurePonDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePonDown    " << endl;

	if (m_pon_set_point > m_pr_params->p_on_min)
	// update the set point
	updatePonSetPoint(m_pon_set_point - 1.0);
}

void Labonatip_GUI::pressurePonUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePonUp    " << endl;

	if (m_pon_set_point < m_pr_params->p_on_max)
	// update the set point
	updatePonSetPoint(m_pon_set_point + 1.0);

}

void Labonatip_GUI::pressurePoffDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePoffDown    " << endl;

	if (m_poff_set_point > m_pr_params->p_off_min)
	// update the set point
	updatePoffSetPoint(m_poff_set_point - 1.0);

}

void Labonatip_GUI::pressurePoffUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePoffUp    " << endl;

	if (m_poff_set_point < m_pr_params->p_off_max)
	// update the set point
	updatePoffSetPoint(m_poff_set_point + 1.0); 

}

void Labonatip_GUI::pressButtonPressed_switchDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressButtonPressed_switchDown    " << endl;

	if (m_v_switch_set_point > m_pr_params->v_switch_max)
	// update the set point
	updateVswitchSetPoint(m_v_switch_set_point - 1.0);

}

void Labonatip_GUI::pressButtonPressed_switchUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressButtonPressed_switchUp    " << endl;

	if (m_v_switch_set_point < -m_pr_params->v_switch_min)
	// update the set point
	updateVswitchSetPoint(m_v_switch_set_point + 1.0);

}

void Labonatip_GUI::recirculationDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::recirculationDown    " << endl;

	if (m_v_recirc_set_point > m_pr_params->v_recirc_max)
		// update the set point
	updateVrecircSetPoint(m_v_recirc_set_point - 1.0);

}

void Labonatip_GUI::recirculationUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::recirculationUp    " << endl;

	if (m_v_recirc_set_point < -m_pr_params->v_recirc_min)
	// update the set point
	updateVrecircSetPoint(m_v_recirc_set_point + 1.0);

}

void Labonatip_GUI::sliderPonChanged(int _value) {

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_on->setValue(_value);
		updateFlowControlPercentages();
	}

	updatePonSetPoint(_value);

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledPon->setValue(0); // turn the led off
		return;
	}
	ui->progressBar_ledPon->setValue(m_ppc1->m_PPC1_data->channel_D->state); // turn the led on

}


void Labonatip_GUI::sliderPoffChanged(int _value) {

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_off->setValue(_value);
		updateFlowControlPercentages();
	}

	updatePoffSetPoint(_value);

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledPoff->setValue(0); // turn the led off
		return;
	}
	ui->progressBar_ledPoff->setValue(m_ppc1->m_PPC1_data->channel_C->state); // turn the led on
}


void Labonatip_GUI::sliderRecircChanged(int _value) {

	if (m_simulationOnly) {

		ui->progressBar_recirc->setValue(_value);
		updateFlowControlPercentages();
	}

	// this updates the bars in the pipette drawing
	//ui->progressBar_recircIn->setValue(_value);
	//ui->progressBar_recircOut->setValue(_value);

	updateVrecircSetPoint(_value);

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledRecirc->setValue(0);  // turn the led off
		return;
	}
	ui->progressBar_ledRecirc->setValue(m_ppc1->m_PPC1_data->channel_A->state); // turn the led on

}


void Labonatip_GUI::sliderSwitchChanged(int _value) {

	if (m_simulationOnly) {
		ui->progressBar_switch->setValue(_value);
		updateFlowControlPercentages();
	}

	// this updates the bars in the pipette drawing
	//ui->progressBar_switchIn->setValue(_value);
	//ui->progressBar_switchOut->setValue(_value);

	updateVswitchSetPoint(_value);

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledSwitch->setValue(0);
		return;
	}
	ui->progressBar_ledSwitch->setValue(m_ppc1->m_PPC1_data->channel_B->state);

}