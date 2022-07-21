/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip.h"

void Labonatip_GUI::zoneSizePlus() 
{
	// only Pon + percentage
	// V_recirc - percentage
	std::cout << HERE << std::endl;

	// check for out of bound percentage values before doing any action
	if (m_ds_perc < 0 || m_ds_perc > MAX_ZONE_SIZE_PERC) {
		QMessageBox::information(this, m_str_warning,
			m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		return;
	}

	// if the pipette is active the command will be sent to the PPC1
	if (m_pipette_active) {

		// use a flag to store success operations
		bool success = false;
		if (m_pr_params->useDefValSetPoint )  
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			success = m_ppc1->changeZoneSizePercBy(m_pr_params->base_ds_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			success = m_ppc1->setZoneSizePerc( m_ds_perc + 
				m_pr_params->base_ds_increment);
		}

		// if not success, the user need to be warned and the function ends
		if(!success) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
			return;
		}

		// finally we update set points and GUI
		// update the set point
		m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
		m_pipette_status->pon_set_point = m_ppc1->getPonSetPoint();

		// update the slider for the GUI
		ui->horizontalSlider_recirculation->blockSignals(true);
		ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
		ui->horizontalSlider_recirculation->blockSignals(false);
		ui->horizontalSlider_p_on->blockSignals(true);
		ui->horizontalSlider_p_on->setValue(m_pipette_status->pon_set_point);
		ui->horizontalSlider_p_on->blockSignals(false);
		
	}

	// if we are running a simulation the commands will not be sent to the PPC1
	if (m_simulationOnly) {

		// check for out of bound values, the recirculation will be increased 
		// (from a negative value), so if it is already 0, nothing can be done
		if (ui->horizontalSlider_recirculation->value() == 0) {
			QMessageBox::information(this, m_str_warning,
				" Recirculation zero, " + m_str_operation_cannot_be_done);
			return;
		}
		

		if (m_pr_params->useDefValSetPoint)
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			changeZoneSizePercentageBy(m_pr_params->base_ds_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			setZoneSizePercentage(m_ds_perc +
				m_pr_params->base_ds_increment);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::zoneSizeMinus() 
{
	// only Pon - percentage
	// V_recirc + percentage
	std::cout << HERE << std::endl;

	// check for out of bound percentage values before doing any action
	if ( m_ds_perc < MIN_ZONE_SIZE_PERC) {
		QMessageBox::information(this, m_str_warning,
			m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		return;
	}

	// if the pipette is active the command will be sent to the PPC1
	if (m_pipette_active) {

		// use a flag to store success operations
		bool success = false;
		if (m_pr_params->useDefValSetPoint)  
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			success = m_ppc1->changeZoneSizePercBy(-m_pr_params->base_ds_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			success = m_ppc1->setZoneSizePerc( m_ds_perc -
				m_pr_params->base_ds_increment);
		}

		// if not success, the user need to be warned and the function ends
		if (!success){
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound); 
			return;
		}

		// update the set point
		m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
		m_pipette_status->pon_set_point = m_ppc1->getPonSetPoint();

		// update the slider for the GUI
		ui->horizontalSlider_recirculation->blockSignals(true);
		ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
		ui->horizontalSlider_recirculation->blockSignals(false);
		ui->horizontalSlider_p_on->blockSignals(true);
		ui->horizontalSlider_p_on->setValue(m_pipette_status->pon_set_point);
		ui->horizontalSlider_p_on->blockSignals(false);
	}

	// if we are running a simulation the commands will not be sent to the PPC1
	if (m_simulationOnly) {

		// check for out of bound values, the pressure_on will be decreased 
		// (from a negative value), so if it is already 0, nothing can be done
		if (ui->horizontalSlider_p_on->value() == 0) {
			QMessageBox::information(this, m_str_warning, 
				" P_on zero, " + m_str_operation_cannot_be_done);
			return;
		}

		if (m_pr_params->useDefValSetPoint)
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			changeZoneSizePercentageBy(-m_pr_params->base_ds_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			setZoneSizePercentage(m_ds_perc -
				m_pr_params->base_ds_increment);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::flowSpeedPlus() 
{

	// +percentage to all values
	// Poff does not read too low values, 
	// if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	std::cout << HERE << std::endl;

	// check for out of bound values
	if (m_fs_perc > MAX_FLOW_SPEED_PERC ) {
		QMessageBox::information(this, m_str_warning,
			m_str_operation_cannot_be_done);
		return;
	}

	// if the pipette is active the command will be sent to the PPC1
	if (m_pipette_active) {
		bool success = false;
		if (m_pr_params->useDefValSetPoint)  
		{
			success = m_ppc1->changeFlowSpeedPercBy(m_pr_params->base_fs_increment);
		}
		else{
			success = m_ppc1->setFlowSpeedPerc(m_fs_perc +
				m_pr_params->base_fs_increment);
		}

		if (!success) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
			return;
		}

		// update the set point
		m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
		m_pipette_status->v_switch_set_point = -m_ppc1->getVswitchSetPoint();
		m_pipette_status->poff_set_point = m_ppc1->getPoffSetPoint();
		m_pipette_status->pon_set_point = m_ppc1->getPonSetPoint();

		// update the slider for the GUI
		ui->horizontalSlider_recirculation->blockSignals(true);
		ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
		ui->horizontalSlider_recirculation->blockSignals(false);
		ui->horizontalSlider_switch->blockSignals(true);
		ui->horizontalSlider_switch->setValue(m_pipette_status->v_switch_set_point);
		ui->horizontalSlider_switch->blockSignals(false);
		ui->horizontalSlider_p_off->blockSignals(true);
		ui->horizontalSlider_p_off->setValue(m_pipette_status->poff_set_point);
		ui->horizontalSlider_p_off->blockSignals(false);
		ui->horizontalSlider_p_on->blockSignals(true);
		ui->horizontalSlider_p_on->setValue(m_pipette_status->pon_set_point);
		ui->horizontalSlider_p_on->blockSignals(false);
		
	}

	// if we are running a simulation the commands will not be sent to the PPC1
	if (m_simulationOnly) {
		
		if (m_pr_params->useDefValSetPoint)
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			changeFlowspeedPercentageBy(m_pr_params->base_fs_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			setFlowspeedPercentage(m_fs_perc +
				m_pr_params->base_fs_increment);
		}

		updateFlowControlPercentages();
		
	}
}

void Labonatip_GUI::flowSpeedMinus() {

	std::cout << HERE << std::endl;

	// check for out of bound values
	if (m_fs_perc < MIN_FLOW_SPEED_PERC) {
		QMessageBox::information(this, m_str_warning,
			m_str_operation_cannot_be_done);
		return;
	}

	// -percentage to all values
	if (m_pipette_active) {
		bool success = false;
		if (m_pr_params->useDefValSetPoint)  
		{
			success = m_ppc1->changeFlowSpeedPercBy(-m_pr_params->base_fs_increment);
		}
		else {
			success = m_ppc1->setFlowSpeedPerc(m_fs_perc -
				m_pr_params->base_fs_increment);
		}

		if (!success) {
				QMessageBox::information(this, m_str_warning, 
					m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		}
		else
		{
			// update the set point
			m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
			m_pipette_status->v_switch_set_point = -m_ppc1->getVswitchSetPoint();
			m_pipette_status->poff_set_point = m_ppc1->getPoffSetPoint();
			m_pipette_status->pon_set_point = m_ppc1->getPonSetPoint();

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
			ui->horizontalSlider_switch->blockSignals(true);
			ui->horizontalSlider_switch->setValue(m_pipette_status->v_switch_set_point);
			ui->horizontalSlider_switch->blockSignals(false);
			ui->horizontalSlider_p_off->blockSignals(true);
			ui->horizontalSlider_p_off->setValue(m_pipette_status->poff_set_point);
			ui->horizontalSlider_p_off->blockSignals(false);
			ui->horizontalSlider_p_on->blockSignals(true);
			ui->horizontalSlider_p_on->setValue(m_pipette_status->pon_set_point);
			ui->horizontalSlider_p_on->blockSignals(false);
		}
	}
	if (m_simulationOnly) {
		
		if (m_pr_params->useDefValSetPoint)
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			changeFlowspeedPercentageBy(-m_pr_params->base_fs_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			setFlowspeedPercentage(m_fs_perc -
				m_pr_params->base_fs_increment);
		}
		
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumPlus() {

	std::cout << HERE << std::endl;

	// +percentage to v_recirculation
	if (m_pipette_active) {
		bool success = false;
		if (m_pr_params->useDefValSetPoint )  
		{
			success = m_ppc1->changeVacuumPercBy(m_pr_params->base_v_increment);
		}
		else {
			success = m_ppc1->setVacuumPerc(m_v_perc + 
				m_pr_params->base_v_increment);
		}

		if (!success) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		}
		else {
			// update the set point
			m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);
		}
	}
	if (m_simulationOnly) {

		if (m_pr_params->useDefValSetPoint)
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			changeVacuumPercentageBy(m_pr_params->base_v_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			double perc = m_v_perc + m_pr_params->base_v_increment;
			this->setVacuumPercentage(perc);
		}
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumMinus() {

	std::cout << HERE << std::endl;

	// -x% v_recirculation
	if (m_pipette_active) {
		bool success = false; 
		if (m_pr_params->useDefValSetPoint) 
		{
			success = m_ppc1->changeVacuumPercBy(-m_pr_params->base_v_increment);
		}
		else {
			success = m_ppc1->setVacuumPerc(m_v_perc - 
				m_pr_params->base_v_increment);
		}

		if (!success) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		}
		else {
			// update the set point
			m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);

		}
	}
	if (m_simulationOnly) {

		if (m_pr_params->useDefValSetPoint)
		{
			// if we are using the set point values, the zone size
			// will be increased by a percentage defined in the tools
			changeVacuumPercentageBy(-m_pr_params->base_v_increment);
		}
		else {
			// if we are using the default values, the zone size
			// will be modified by setting the values at a specific percentage 
			double perc = m_v_perc - m_pr_params->base_v_increment;
			this->setVacuumPercentage(perc);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::setZoneSizePercentage(double _perc)
{
	std::cout << HERE << "  " << _perc << std::endl;

	if (m_simulationOnly) {
		
		if (_perc < MIN_ZONE_SIZE_PERC || _perc > MAX_ZONE_SIZE_PERC ) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}

		double perc = _perc/ 100.0;

		double delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
		double value = -m_pr_params->v_recirc_default -
				m_pr_params->v_recirc_default * delta;

		updateVrecircSetPoint(value);
		updateVswitchSetPoint(-m_pr_params->v_recirc_default);
		
		delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
		value = m_pr_params->p_on_default - m_pr_params->p_on_default  * delta;

		updatePonSetPoint(value);
		updatePoffSetPoint(m_pr_params->p_off_default);
		
	}
}

void Labonatip_GUI::changeZoneSizePercentageBy(double _perc)
{
	std::cout << HERE << "  " << _perc << std::endl;

	if (m_simulationOnly) {

		if ( std::abs(_perc) > MAX_ZONE_SIZE_INCREMENT) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}
		//calculate the new percentage
		double increment = (100.0 +	_perc) / 100.0;
		double delta = (1.0 - std::pow(increment, (1.0 / 3.0)));
		double value = m_pipette_status->v_recirc_set_point -
			m_pr_params->v_recirc_default * delta;

		updateVrecircSetPoint(value);

		if (m_pipette_status->pon_set_point == 0) {
			updatePonSetPoint(3.0);
			return;
		}

		value = m_pipette_status->pon_set_point - 
			m_pr_params->p_on_default  * delta;

		updatePonSetPoint(value);
	}

	updateFlowControlPercentages();
}

void Labonatip_GUI::setFlowspeedPercentage(double _perc)
{
	std::cout << HERE << "  " << _perc << std::endl;

	if (m_simulationOnly) {
		if (_perc < MIN_FLOW_SPEED_PERC || _perc > MAX_FLOW_SPEED_PERC) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}

		double value = m_pr_params->p_on_default *  _perc / 100.0;
		updatePonSetPoint(value);

		value = m_pr_params->p_off_default * _perc / 100.0;
		updatePoffSetPoint(value);

		value = -m_pr_params->v_switch_default * _perc / 100.0;
		updateVswitchSetPoint(value);

		value = -m_pr_params->v_recirc_default * _perc / 100.0;
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::changeFlowspeedPercentageBy(double _perc)
{
	if (m_simulationOnly) {
		if (std::abs(_perc) > MAX_FLOW_SPEED_INCREMENT) {
			//if (m_fs_perc > MAX_FLOW_SPEED_PERC - m_pr_params->base_fs_increment) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}

		double value;
		value = m_pipette_status->pon_set_point +
			m_pr_params->p_on_default * _perc / 100.0;
		updatePonSetPoint(value);

		value = m_pipette_status->poff_set_point +
			m_pr_params->p_off_default * _perc / 100.0;
		updatePoffSetPoint(value);

		value = m_pipette_status->v_switch_set_point -
			m_pr_params->v_switch_default * _perc / 100.0;
		updateVswitchSetPoint(value);

		value = m_pipette_status->v_recirc_set_point -
			m_pr_params->v_recirc_default * _perc / 100.0;
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}


}

void Labonatip_GUI::setVacuumPercentage(double _perc)
{
	std::cout << HERE << "  " << _perc << std::endl;

	if (m_simulationOnly) {

		if (_perc < MIN_VACUUM_PERC || _perc > MAX_VACUUM_PERC) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}	

		double value = -m_pr_params->v_recirc_default * _perc/100.0;
		
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::changeVacuumPercentageBy(double _perc)
{
	std::cout << HERE << "  " << _perc << std::endl;

	if (m_simulationOnly) {

		if (std::abs(_perc) > MAX_VACUUM_INCREMENT) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}

		double value = m_pipette_status->v_recirc_set_point - 
			m_pr_params->v_recirc_default * _perc / 100.0;
		updateVrecircSetPoint(value);

		updateFlowControlPercentages();
	}

}

void Labonatip_GUI::buildDPmap()
{

	// the row is: pon poff vs vr zs fs
	// zs = zone size
	// fs = flow speed
	typedef std::vector<double> DP_row;
	typedef std::vector<DP_row> DP_table;

	DP_table table;

	QFile tableFile("./table.txt");
	QTextStream stream(&tableFile);
	QString header;
	header = "pon poff vs vr zs fs\n";
	if (tableFile.open(QIODevice::WriteOnly))
	{
	stream << header;

	//for (int pon_idx = 0; pon_idx < MAX_CHAN_D; pon_idx++)
	for (int pon_idx = 170; pon_idx < 220; pon_idx++)
	{
		updatePonSetPoint(pon_idx);

		//for (int poff_idx = 0; poff_idx < MAX_CHAN_C; poff_idx++)
		for (int poff_idx = 15; poff_idx < 30; poff_idx++)
		{
			updatePoffSetPoint(poff_idx);

			//for (int vs_idx = 0; vs_idx > MIN_CHAN_B; vs_idx--)
			for (int vs_idx = -100; vs_idx > -130; vs_idx--)
			{
				updateVswitchSetPoint(-vs_idx);

				//for (int vr_idx = 0; vr_idx > MIN_CHAN_A; vr_idx--)
				for (int vr_idx = -100; vr_idx > -130; vr_idx--)
				{
					updateVrecircSetPoint(-vr_idx);

					//updateFlowControlPercentages();

					DP_row new_row;
					new_row.push_back(pon_idx);
					new_row.push_back(poff_idx);
					new_row.push_back(vs_idx);
					new_row.push_back(vr_idx);
					new_row.push_back(m_ds_perc);
					new_row.push_back(m_fs_perc);

					table.push_back(new_row);

					if (m_ds_perc > 0 && m_ds_perc < 500 &&
						m_fs_perc > 0 && m_fs_perc < 500)
					{
						stream << pon_idx << " "
							<< poff_idx << " "
							<< vs_idx << " "
							<< vr_idx << " "
							<< m_ds_perc << " "
							<< m_fs_perc << " " << "\n";
					}
					//QThread::msleep(100);
				}
			}
		}

	}

	}
}





void Labonatip_GUI::updateFlowControlPercentages()
{
	updateFlows();
	if (m_simulationOnly) {

		// calculate droplet size percentage
		m_ds_perc = 100.0*(m_pipette_status->in_out_ratio_on + 0.21) / 0.31;
		if (m_ds_perc < 0 || m_ds_perc > 1000) {
			ui->lcdNumber_dropletSize_percentage->display(display_e);
		}
		else {
			ui->lcdNumber_dropletSize_percentage->display(m_ds_perc);
		}
		
		// calculate flow speed percentage
		double ponp = 100.0 * m_pipette_status->pon_set_point / m_pr_params->p_on_default;

		//MB: mod to consider pon only in the calculation of the speed
		m_fs_perc = ponp;
		ui->lcdNumber_flowspeed_percentage->display(m_fs_perc);

		//calculate vacuum percentage
		m_v_perc = 100.0 * m_pipette_status->v_recirc_set_point / (-m_pr_params->v_recirc_default);
		ui->lcdNumber_vacuum_percentage->display(m_v_perc);

	}
}
