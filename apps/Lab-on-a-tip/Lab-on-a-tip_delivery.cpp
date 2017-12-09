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

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::dropletSizePlus    " << endl;

	// only Pon + percentage
	// V_recirc - percentage
	if (m_pipette_active) {
		bool success = false;
		if (ui->checkBox_useSetPoint->isChecked())
		{
			success = m_ppc1->changeDropletSizeBy(m_dialog_tools->m_pr_params->base_ds_increment);
		}
		else {
			success = m_ppc1->setDropletSize(
				m_ds_perc + //ui->lcdNumber_dropletSize_percentage->value() +
				m_dialog_tools->m_pr_params->base_ds_increment);
		}
		//if (!m_ppc1->setDropletSize(ui->lcdNumber_dropletSize_percentage->value() +
		//	m_dialog_tools->m_pr_params->base_ds_increment)) {
		if(!success) {
			QMessageBox::information(this, "Warning ",
				" Operation cannot be done. <br> Please, check for out of bound values. ");
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
		double perc = (m_ds_perc + 
			m_dialog_tools->m_pr_params->base_ds_increment) / 100.0;

		if (ui->horizontalSlider_recirculation->value() == 0) {
			QMessageBox::information(this, "Warning ", 
				" Recirculation zero, operation cannot be done ");
			return;
		}
		else {
			double value = default_v_recirc + default_v_recirc * (1.0 -
				std::pow(perc, (1.0 / 3.0)));
			//TODO: the new value must consider the current set point!!!
			//double value = m_v_recirc_set_point + (default_v_recirc * 
			//	(2.0 - std::pow(1.0 + m_dialog_tools->m_pr_params->base_ds_increment/100.0, (1.0 / 3.0))) - default_v_recirc);

			updateVrecircSetPoint(value);
		}

		if (ui->horizontalSlider_p_on->value() == 0) {
			updatePonSetPoint(3.0);
		}
		else {
			double value = default_pon - default_pon * (1.0 -
				std::pow(perc, (1.0 / 3.0)));
			//TODO: the new value must consider the current set point!!!
			//double value = m_pon_set_point + (default_pon * 
			//	( std::pow(1.0 + m_dialog_tools->m_pr_params->base_ds_increment / 100.0, (1.0 / 3.0))) - default_pon);

			updatePonSetPoint(value);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::dropletSizeMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::dropletSizeMinus    " << endl;

	// only Pon - percentage
	// V_recirc + percentage
	if (m_pipette_active) {
		bool success = false;
		if (ui->checkBox_useSetPoint->isChecked())
		{
			success = m_ppc1->changeDropletSizeBy(-m_dialog_tools->m_pr_params->base_ds_increment);
		}
		else {
			success = m_ppc1->setDropletSize( m_ds_perc -
				m_dialog_tools->m_pr_params->base_ds_increment);
		}
		//if (!m_ppc1->setDropletSize(
		//		ui->lcdNumber_dropletSize_percentage->value() -
		//		m_dialog_tools->m_pr_params->base_ds_increment)) {
		if (!success){
			QMessageBox::information(this, "Warning ",
				" Operation cannot be done. <br> Please, check for out of bound values. ");
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
		double perc = (m_ds_perc - 
			m_dialog_tools->m_pr_params->base_ds_increment) / 100.0;

		if (ui->horizontalSlider_p_on->value() == 0) {
			QMessageBox::information(this, "Warning ", 
				" P_on zero, operation cannot be done ");
			return;
		}
		else {
			double value = default_pon - default_pon * ( 1.0 -
				std::pow(perc, (1.0/3.0) )); //TODO: this must be explained
			//TODO: the new value must consider the current set point!!!

			//double value = m_pon_set_point + (default_pon *
			//	(std::pow(1.0 - m_dialog_tools->m_pr_params->base_ds_increment / 100.0, (1.0 / 3.0))) - default_pon);
			updatePonSetPoint(value);
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			updateVrecircSetPoint(-3.0);
		}
		else {
			double value = default_v_recirc + default_v_recirc * (1.0 -
				std::pow(perc, (1.0 / 3.0)));
			//TODO: the new value must consider the current set point!!!
			//double value = m_v_recirc_set_point + (default_v_recirc *
			//	(2.0 - std::pow(1.0 - m_dialog_tools->m_pr_params->base_ds_increment / 100.0, (1.0 / 3.0))) - default_v_recirc);

			updateVrecircSetPoint(value);
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedPlus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::flowSpeedPlus    " << endl;

	// +percentage to all values
	// Poff does not read too low values, 
	// if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	if (m_pipette_active) {
		bool success = false;
		if (ui->checkBox_useSetPoint->isChecked())
		{
			success = m_ppc1->changeFlowspeedBy(m_dialog_tools->m_pr_params->base_fs_increment);
		}
		else{
			success = m_ppc1->setFlowspeed(m_fs_perc +
				m_dialog_tools->m_pr_params->base_fs_increment);
		}
		//if (!m_ppc1->changeFlowspeedBy(m_dialog_tools->m_pr_params->base_fs_increment)) {
		//if (!m_ppc1->setFlowspeed(ui->lcdNumber_flowspeed_percentage->value() +
		//	m_dialog_tools->m_pr_params->base_fs_increment)) 
		if (!success) {
			QMessageBox::information(this, "Warning ", 
				" Operation cannot be done. <br> Please, check for out of bound values. ");
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
			double value = m_pon_set_point + 
				default_pon *  m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
			updatePonSetPoint(value);
		}

		if (ui->horizontalSlider_p_off->value() == 0) {
			updatePoffSetPoint(5.0);
		}
		else {
			double value = m_poff_set_point + 
				default_poff * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
			updatePoffSetPoint(value);
		}

		if (ui->horizontalSlider_switch->value() == 0) {
			updateVswitchSetPoint ( 5.0 );
		}
		else {
			double value = m_v_switch_set_point + 
				default_v_switch * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
			updateVswitchSetPoint(value);
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			updateVrecircSetPoint(5.0);
		}
		else {
			double value = m_v_recirc_set_point + 
				default_v_recirc * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
			updateVrecircSetPoint(value);
		}
		updateFlowControlPercentages();

	}
}

void Labonatip_GUI::flowSpeedMinus() {


	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::flowSpeedMinus    " << endl;

	// -percentage to all values
	if (m_pipette_active) {
		bool success = false;
		if (ui->checkBox_useSetPoint->isChecked())
		{
			success = m_ppc1->changeFlowspeedBy(-m_dialog_tools->m_pr_params->base_fs_increment);
		}
		else {
			success = m_ppc1->setFlowspeed(m_fs_perc -
				m_dialog_tools->m_pr_params->base_fs_increment);
		}
		//if (!m_ppc1->changeFlowspeedBy(-m_dialog_tools->m_pr_params->base_fs_increment)){
		//if (!m_ppc1->setFlowspeed(ui->lcdNumber_flowspeed_percentage->value() -
		//		m_dialog_tools->m_pr_params->base_fs_increment)) 
		if (!success) {
				QMessageBox::information(this, "Warning ",
				" Operation cannot be done. <br> Please, check for out of bound values. ");
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
		
		//double perc = (m_fs_perc - //ui->lcdNumber_flowspeed_percentage->value() -
		//	m_dialog_tools->m_pr_params->base_fs_increment) / 100.0;

		double value = m_pon_set_point - default_pon * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
		updatePonSetPoint(value);
		value = m_poff_set_point - default_poff * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
		updatePoffSetPoint(value);
		value = m_v_switch_set_point - default_v_switch * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
		updateVswitchSetPoint(value);
		value = m_v_recirc_set_point - default_v_recirc * m_dialog_tools->m_pr_params->base_fs_increment / 100.0;
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumPlus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::vacuumPlus    " << endl;

	// +percentage to v_recirculation
	if (m_pipette_active) {
		bool success = false;
		if (ui->checkBox_useSetPoint->isChecked())
		{
			success = m_ppc1->changeVacuumPercentageBy(m_dialog_tools->m_pr_params->base_v_increment);
		}
		else {
			success = m_ppc1->setVacuumPercentage(m_v_perc + //ui->lcdNumber_vacuum_percentage->value() +
				m_dialog_tools->m_pr_params->base_v_increment);
		}
		//if (!m_ppc1->changeVacuumPercentageBy(m_dialog_tools->m_pr_params->base_v_increment)){
		//if (!m_ppc1->setVacuumPercentage(ui->lcdNumber_vacuum_percentage->value() + 
		//	m_dialog_tools->m_pr_params->base_v_increment)) {
		if (!success) {
			QMessageBox::information(this, "Warning ",
				" Operation cannot be done. <br> Please, check for out of bound values.");
		}
		else {
			// update the set point
			m_v_recirc_set_point = -m_ppc1->m_PPC1_data->channel_A->set_point;

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

			double value = m_v_recirc_set_point + default_v_recirc * m_dialog_tools->m_pr_params->base_v_increment / 100.0;
			cout << "Labonatip_GUI::vacuumPlus    ::: new recirculation value " << value << endl;
			updateVrecircSetPoint(value);
			
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::vacuumMinus    " << endl;

	// -5% v_recirculation
	if (m_pipette_active) {
		bool success = false; 
		if (ui->checkBox_useSetPoint->isChecked())
		{
			success = m_ppc1->changeVacuumPercentageBy(-m_dialog_tools->m_pr_params->base_v_increment);
		}
		else {
			success = m_ppc1->setVacuumPercentage(m_v_perc - //ui->lcdNumber_vacuum_percentage->value() -
				m_dialog_tools->m_pr_params->base_v_increment);
		}

		//if (!m_ppc1->changeVacuumPercentageBy(-m_dialog_tools->m_pr_params->base_v_increment)){
		//if (!m_ppc1->setVacuumPercentage(ui->lcdNumber_vacuum_percentage->value() -
		//	m_dialog_tools->m_pr_params->base_v_increment)) {
		if (!success) {
			QMessageBox::information(this, "Warning ",
				" Operation cannot be done. <br> Please, check for out of bound values. ");
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
			QMessageBox::information(this, "Warning ", 
				" Operation cannot be done. <br> Recirculation is zero. ");
		}

		double value = m_v_recirc_set_point - default_v_recirc * m_dialog_tools->m_pr_params->base_v_increment / 100.0;
		cout << "Labonatip_GUI::vacuumMinus    ::: new recirculation value " << value << endl;
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::updateFlowControlPercentages()
{

	if (m_simulationOnly) {

		// calculate droplet size percentage
		{
			//double ponp = 100.0 * m_pon_set_point / default_pon;
			//double vrp = 100.0 * m_v_recirc_set_point / default_v_recirc;

			//double droplet_percentage = 100.0 + (ponp - vrp) / 2.0;
			
			double ponp =  100.0 * std::pow(m_pon_set_point / default_pon, 3.0);
			double vrp =  -100.0 * std::pow((m_v_recirc_set_point - 2.0 * default_v_recirc) / default_v_recirc, 3.0);

			//double droplet_percentage = std::pow(1.0 + (ponp - vrp) / 2.0, 3);
			m_ds_perc =  (ponp + vrp) / 2.0;
			cout << "Labonatip_GUI::updateFlowControlPercentages  ::: " 
				 << " ponp " << ponp
				 << " vrp " << vrp 
				 << " droplet_percentage " << m_ds_perc << endl;

			ui->lcdNumber_dropletSize_percentage->display(m_ds_perc);
		}

		// calculate flow speed percentage
		{
			double ponp = 100.0 * m_pon_set_point / default_pon; 
			double poffp = 100.0 * m_poff_set_point / default_poff; 
			double vsp = 100.0 * m_v_switch_set_point / default_v_switch; 
			double vrp = 100.0 * m_v_recirc_set_point / default_v_recirc; 

			m_fs_perc = (ponp + poffp + vsp + vrp) / 4.0; // 4 is the number of elements in the average
			ui->lcdNumber_flowspeed_percentage->display(m_fs_perc);
		}

		//calculate vacuum percentage
		{
			m_v_perc = 100.0 * m_v_recirc_set_point / default_v_recirc; 
			ui->lcdNumber_vacuum_percentage->display(m_v_perc);
		}
	}

	ui->progressBar_recircIn->setValue(ui->horizontalSlider_recirculation->value());
	ui->progressBar_recircOut->setValue(ui->horizontalSlider_recirculation->value());

	ui->progressBar_switchIn->setValue(ui->horizontalSlider_switch->value());
	ui->progressBar_switchOut->setValue(ui->horizontalSlider_switch->value());

	//updateDrawing(ui->progressBar_dropletSize->value());
	updateFlows();
}
