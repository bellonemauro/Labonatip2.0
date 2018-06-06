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
	m_pipette_status->pon_set_point = _pon_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updatePonSetPoint   :::: set value  =  " 
		 << _pon_set_point 
		 << " sensor value = " << m_ppc1->channel_D->sensor_reading << endl;

	ui->label_PonPressure->setText(QString(
		QString::number(int(_pon_set_point)) + " mbar    "));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setPressureChannelD(_pon_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_p_on->blockSignals(true);
	ui->horizontalSlider_p_on->setValue(_pon_set_point);
	ui->horizontalSlider_p_on->blockSignals(false);

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_on->setValue(_pon_set_point);
		updateFlowControlPercentages();
		updateGUI();
	}
}

void Labonatip_GUI::updatePoffSetPoint(double _poff_set_point)
{	
	// update the set point
	m_pipette_status->poff_set_point = _poff_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updatePoffSetPoint   :::: set value  =  " 
		 << _poff_set_point
		  << " sensor value = " << m_ppc1->channel_C->sensor_reading << endl;

	ui->label_PoffPressure->setText(QString(
		QString::number(int(_poff_set_point)) + " mbar    "));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setPressureChannelC(_poff_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_p_off->blockSignals(true);
	ui->horizontalSlider_p_off->setValue(_poff_set_point);
	ui->horizontalSlider_p_off->blockSignals(false);

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_off->setValue(_poff_set_point);
		updateFlowControlPercentages();
		updateGUI();
	}
}

void Labonatip_GUI::updateVrecircSetPoint(double _v_recirc_set_point)
{
	// update the set point
	m_pipette_status->v_recirc_set_point = _v_recirc_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateVrecircSetPoint   :::: set value  =  " 
		 << _v_recirc_set_point
		 << " sensor value = " << m_ppc1->channel_A->sensor_reading << endl;

	ui->label_recircPressure->setText(QString(
		QString::number(-int(_v_recirc_set_point)) + " mbar    "));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setVacuumChannelA(-_v_recirc_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_recirculation->blockSignals(true);
	ui->horizontalSlider_recirculation->setValue(_v_recirc_set_point);
	ui->horizontalSlider_recirculation->blockSignals(false);

	if (m_simulationOnly) {

		ui->progressBar_recirc->setValue(_v_recirc_set_point);
		updateFlowControlPercentages();
		updateGUI();
	}
}

void Labonatip_GUI::updateVswitchSetPoint(double _v_switch_set_point)
{
	// update the set point
	m_pipette_status->v_switch_set_point = _v_switch_set_point;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateVswitchSetPoint   :::: set value  =  " 
		 << _v_switch_set_point
		 << " sensor value = " << m_ppc1->channel_B->sensor_reading << endl;

	ui->label_switchPressure->setText(QString(
		QString::number(-int(_v_switch_set_point)) + " mbar    "));

	// if the pipette is active we send the set point to the device
	if (m_pipette_active) {
		m_ppc1->setVacuumChannelB(-_v_switch_set_point);
	}

	// update the slider for the GUI
	ui->horizontalSlider_switch->blockSignals(true);
	ui->horizontalSlider_switch->setValue(_v_switch_set_point);
	ui->horizontalSlider_switch->blockSignals(false);

	if (m_simulationOnly) {
		ui->progressBar_switch->setValue(_v_switch_set_point);
		updateFlowControlPercentages();
		updateGUI();
	}
}


void Labonatip_GUI::pressurePonDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePonDown    " << endl;

	if (m_pipette_status->pon_set_point > m_pr_params->p_on_min)  // check if in range
		// update the set point
		updatePonSetPoint(m_pipette_status->pon_set_point - 1.0);
}

void Labonatip_GUI::pressurePonUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePonUp    " << endl;

	if (m_pipette_status->pon_set_point < m_pr_params->p_on_max)  // check if in range
		// update the set point
		updatePonSetPoint(m_pipette_status->pon_set_point + 1.0);
}

void Labonatip_GUI::pressurePoffDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePoffDown    " << endl;

	if (m_pipette_status->poff_set_point > m_pr_params->p_off_min)  // check if in range
		updatePoffSetPoint(m_pipette_status->poff_set_point - 1.0);
}

void Labonatip_GUI::pressurePoffUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressurePoffUp    " << endl;

	if (m_pipette_status->poff_set_point < m_pr_params->p_off_max)  // check if in range
		updatePoffSetPoint(m_pipette_status->poff_set_point + 1.0);
}

void Labonatip_GUI::pressButtonPressed_switchDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressButtonPressed_switchDown    " << endl;

	if (m_pipette_status->v_switch_set_point > m_pr_params->v_switch_max)  // check if in range
		updateVswitchSetPoint(m_pipette_status->v_switch_set_point - 1.0);
}

void Labonatip_GUI::pressButtonPressed_switchUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::pressButtonPressed_switchUp    " << endl;

	if (m_pipette_status->v_switch_set_point < -m_pr_params->v_switch_min)  // check if in range
		updateVswitchSetPoint(m_pipette_status->v_switch_set_point + 1.0);
}

void Labonatip_GUI::recirculationDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::recirculationDown    " << endl;

	if (m_pipette_status->v_recirc_set_point > m_pr_params->v_recirc_max)  // check if in range
		updateVrecircSetPoint(m_pipette_status->v_recirc_set_point - 1.0);
}

void Labonatip_GUI::recirculationUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::recirculationUp    " << endl;

	if (m_pipette_status->v_recirc_set_point < -m_pr_params->v_recirc_min)  // check if in range
		updateVrecircSetPoint(m_pipette_status->v_recirc_set_point + 1.0);
}

void Labonatip_GUI::sliderPonChanged(int _value) {

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_on->setValue(_value);
		updateFlowControlPercentages();
	}

	updatePonSetPoint(_value);

}


void Labonatip_GUI::sliderPoffChanged(int _value) {

	if (m_simulationOnly) {

		ui->progressBar_pressure_p_off->setValue(_value);
		updateFlowControlPercentages();
	}

	updatePoffSetPoint(_value);
}


void Labonatip_GUI::sliderRecircChanged(int _value) {

	if (m_simulationOnly) {

		ui->progressBar_recirc->setValue(_value);
		updateFlowControlPercentages();
	}

	updateVrecircSetPoint(_value);
}


void Labonatip_GUI::sliderSwitchChanged(int _value) {

	if (m_simulationOnly) {
		ui->progressBar_switch->setValue(_value);
		updateFlowControlPercentages();
	}

	updateVswitchSetPoint(_value);
}

void Labonatip_GUI::setPreset1() {

	updatePonSetPoint(m_pr_params->p_on_preset1);
	updatePoffSetPoint(m_pr_params->p_off_preset1);
	updateVswitchSetPoint(-m_pr_params->v_switch_preset1);
	updateVrecircSetPoint(-m_pr_params->v_recirc_preset1);
}

void Labonatip_GUI::setPreset2() {
	updatePonSetPoint(m_pr_params->p_on_preset2);
	updatePoffSetPoint(m_pr_params->p_off_preset2);
	updateVswitchSetPoint(-m_pr_params->v_switch_preset2);
	updateVrecircSetPoint(-m_pr_params->v_recirc_preset2);
}

void Labonatip_GUI::setPreset3() {
	updatePonSetPoint(m_pr_params->p_on_preset3);
	updatePoffSetPoint(m_pr_params->p_off_preset3);
	updateVswitchSetPoint(-m_pr_params->v_switch_preset3);
	updateVrecircSetPoint(-m_pr_params->v_recirc_preset3);
}

void Labonatip_GUI::resetPreset1() {
	m_dialog_tools->setPreset1(ui->horizontalSlider_p_on->value(),
		ui->horizontalSlider_p_off->value(),
		-ui->horizontalSlider_switch->value(),
		-ui->horizontalSlider_recirculation->value());
	*m_pr_params = m_dialog_tools->getPr_params();
}

void Labonatip_GUI::resetPreset2() {
	m_dialog_tools->setPreset2(ui->horizontalSlider_p_on->value(),
		ui->horizontalSlider_p_off->value(), 
		-ui->horizontalSlider_switch->value(),
		-ui->horizontalSlider_recirculation->value());
	*m_pr_params = m_dialog_tools->getPr_params();
}

void Labonatip_GUI::resetPreset3() {
	m_dialog_tools->setPreset3(ui->horizontalSlider_p_on->value(),
		ui->horizontalSlider_p_off->value(),
		-ui->horizontalSlider_switch->value(),
		-ui->horizontalSlider_recirculation->value());
	*m_pr_params = m_dialog_tools->getPr_params();
}