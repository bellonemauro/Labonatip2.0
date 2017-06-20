/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"




void Labonatip_GUI::pressurePonDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pressurePonDown    " << endl;

	ui->horizontalSlider_p_on->setValue(ui->horizontalSlider_p_on->value() - 1);
}

void Labonatip_GUI::pressurePonUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pressurePonUp    " << endl;

	ui->horizontalSlider_p_on->setValue(ui->horizontalSlider_p_on->value() + 1);
}

void Labonatip_GUI::pressurePoffDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pressurePoffDown    " << endl;

	ui->horizontalSlider_p_off->setValue(ui->horizontalSlider_p_off->value() - 1);
}

void Labonatip_GUI::pressurePoffUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pressurePoffUp    " << endl;

	ui->horizontalSlider_p_off->setValue(ui->horizontalSlider_p_off->value() + 1);
}

void Labonatip_GUI::pressButtonPressed_switchDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pressButtonPressed_switchDown    " << endl;

	ui->horizontalSlider_switch->setValue(ui->horizontalSlider_switch->value() - 1);
}

void Labonatip_GUI::pressButtonPressed_switchUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::pressButtonPressed_switchUp    " << endl;

	ui->horizontalSlider_switch->setValue(ui->horizontalSlider_switch->value() + 1);
}

void Labonatip_GUI::recirculationDown() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::recirculationDown    " << endl;

	ui->horizontalSlider_recirculation->setValue(ui->horizontalSlider_recirculation->value() - 1);
}

void Labonatip_GUI::recirculationUp() {
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::recirculationUp    " << endl;

	ui->horizontalSlider_recirculation->setValue(ui->horizontalSlider_recirculation->value() + 1);
}

void Labonatip_GUI::sliderPonChanged(int _value) {

	if (m_simulationOnly) {
		ui->label_PonPressure->setText(QString(QString::number(_value) + " mbar"));
		ui->progressBar_pressure_p_on->setValue(_value);
		updateFlowControlPercentages();
	}

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) {
		m_ppc1->setPressureChannelD(_value);
	}

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledPon->setValue(0); // turn the led off
		return;
	}
	ui->progressBar_ledPon->setValue(m_ppc1->m_PPC1_data->channel_D->state); // turn the led on

}


void Labonatip_GUI::sliderPoffChanged(int _value) {

	if (m_simulationOnly) {
		ui->label_PoffPressure->setText(QString(QString::number(_value) + " mbar"));
		ui->progressBar_pressure_p_off->setValue(_value);
		updateFlowControlPercentages();
	}



	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) {
		m_ppc1->setPressureChannelC(_value);
	}

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledPoff->setValue(0); // turn the led off
		return;
	}
	ui->progressBar_ledPoff->setValue(m_ppc1->m_PPC1_data->channel_C->state); // turn the led on
}


void Labonatip_GUI::sliderRecircChanged(int _value) {

	if (m_simulationOnly) {
		ui->label_recircPressure->setText(QString(QString::number(-_value) + " mbar"));
		ui->progressBar_recirc->setValue(_value);
		updateFlowControlPercentages();
	}

	ui->progressBar_recircIn->setValue(_value);
	ui->progressBar_recircOut->setValue(_value);

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) {
		m_ppc1->setVacuumChannelA(-_value);
	}

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledRecirc->setValue(0);  // turn the led off
		return;
	}
	ui->progressBar_ledRecirc->setValue(m_ppc1->m_PPC1_data->channel_A->state); // turn the led on

}


void Labonatip_GUI::sliderSwitchChanged(int _value) {

	if (m_simulationOnly) {
		ui->label_switchPressure->setText(QString(QString::number(-_value) + " mbar"));
		ui->progressBar_switch->setValue(_value);
		updateFlowControlPercentages();
	}

	ui->progressBar_switchIn->setValue(_value);
	ui->progressBar_switchOut->setValue(_value);

	// SET vacum to _value
	// TODO: be careful the control is not implemented, the value will be sent directly ! 
	if (m_pipette_active) {
		m_ppc1->setVacuumChannelB(-_value);
	}

	// if we decrease the pressure to zero, clean the scene and return, so the flow disappear
	if (_value == 0) {
		ui->progressBar_ledSwitch->setValue(0);
		return;
	}
	ui->progressBar_ledSwitch->setValue(m_ppc1->m_PPC1_data->channel_B->state);

}