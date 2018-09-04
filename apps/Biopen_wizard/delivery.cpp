/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip.h"

void Labonatip_GUI::dropletSizePlus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::dropletSizePlus    " << endl;

	//TODO: check for negative percentage values before doing any action
	if (m_ds_perc < 0)
	{
		QMessageBox::information(this, m_str_warning,
			m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		return;
	}

	// only Pon + percentage
	// V_recirc - percentage
	if (m_pipette_active) {
		bool success = false;
		if (m_pr_params->useDefValSetPoint)
		{
			success = m_ppc1->changeDropletSizeBy(m_pr_params->base_ds_increment);
		}
		else {
			success = m_ppc1->setDropletSize(
				m_ds_perc + 
				m_pr_params->base_ds_increment);
		}

		if(!success) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		}
		else
		{
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
	}
	if (m_simulationOnly) {

		if (m_ds_perc > MAX_ZONE_SIZE_PERC - 2*m_pr_params->base_ds_increment) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done); 
			return;
		}

		double perc = (m_ds_perc + 
			m_pr_params->base_ds_increment) / 100.0;

		if (ui->horizontalSlider_recirculation->value() == 0) {
			QMessageBox::information(this, m_str_warning,
				" Recirculation zero, " + m_str_operation_cannot_be_done);
			return;
		}
		else {
			double delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->v_recirc_set_point -
					m_pr_params->v_recirc_default * delta;
			}
			else
			{
				value = -m_pr_params->v_recirc_default -
					m_pr_params->v_recirc_default * delta;
			}

			updateVrecircSetPoint(value);
		}

		if (ui->horizontalSlider_p_on->value() == 0) {
			updatePonSetPoint(3.0);
		}
		else {
			double delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->pon_set_point - m_pr_params->p_on_default  * delta;
			}
			else
			{
				value = m_pr_params->p_on_default - m_pr_params->p_on_default  * delta;
			}

			updatePonSetPoint(value);
		}

		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::dropletSizeMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::dropletSizeMinus    " << endl;

	//TODO: check for negative percentage values before doing any action
	if (m_ds_perc < 0)
	{
		QMessageBox::information(this, m_str_warning,
			m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		return;
	}

	// only Pon - percentage
	// V_recirc + percentage
	if (m_pipette_active) {
		bool success = false;
		if (m_pr_params->useDefValSetPoint)
		{
			success = m_ppc1->changeDropletSizeBy(-m_pr_params->base_ds_increment);
		}
		else {
			success = m_ppc1->setDropletSize( m_ds_perc -
				m_pr_params->base_ds_increment);
		}

		if (!success){
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound); 
		}
		else
		{
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
	}
	if (m_simulationOnly) {
	
		if (m_ds_perc < MIN_ZONE_SIZE_PERC + 2*m_pr_params->base_ds_increment) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done); 
			return;
		}
		
		double perc = (m_ds_perc -
			m_pr_params->base_ds_increment) / 100.0;

		if (ui->horizontalSlider_p_on->value() == 0) {
			QMessageBox::information(this, m_str_warning, 
				" P_on zero, " + m_str_operation_cannot_be_done);
			return;
		}
		else {
			double delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->pon_set_point - m_pr_params->p_on_default  * delta;
			}
			else
			{
				value = m_pr_params->p_on_default - m_pr_params->p_on_default  * delta;
			}
			   // value = m_pr_params->p_on_default - m_pr_params->p_on_default  * delta; 

			updatePonSetPoint(value);
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			updateVrecircSetPoint(-3.0);
		}
		else {
			double delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->v_recirc_set_point -
					m_pr_params->v_recirc_default * delta;
			}
			else
			{
				value = -m_pr_params->v_recirc_default -
					m_pr_params->v_recirc_default * delta;
			}
			//  value = -m_pr_params->v_recirc_default - m_pr_params->v_recirc_default * delta;

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
		if (m_pr_params->useDefValSetPoint)
		{
			success = m_ppc1->changeFlowspeedBy(m_pr_params->base_fs_increment);
		}
		else{
			success = m_ppc1->setFlowspeed(m_fs_perc +
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
		
		if (m_fs_perc > MAX_FLOW_SPEED_PERC - m_pr_params->base_fs_increment) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done); 
			return;
		}

		if (ui->horizontalSlider_p_on->value() == 0) {
			updatePonSetPoint(5.0);
		}
		else {
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->pon_set_point +
					m_pr_params->p_on_default *  m_pr_params->base_fs_increment / 100.0;
			}
			else
			{
				double perc = (m_fs_perc + m_pr_params->base_fs_increment) / 100.0;
				value = m_pr_params->p_on_default * perc;
			}
			updatePonSetPoint(value);

		}

		if (ui->horizontalSlider_p_off->value() == 0) {
			updatePoffSetPoint(5.0);
		}
		else {
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->poff_set_point +
					m_pr_params->p_off_default * m_pr_params->base_fs_increment / 100.0;
			}
			else
			{
				double perc = (m_fs_perc + m_pr_params->base_fs_increment) / 100.0;
				value = m_pr_params->p_off_default * perc;
			}
			updatePoffSetPoint(value);
		}

		if (ui->horizontalSlider_switch->value() == 0) {
			updateVswitchSetPoint ( 5.0 );
		}
		else {
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->v_switch_set_point -
					m_pr_params->v_switch_default * m_pr_params->base_fs_increment / 100.0;
			}
			else
			{
				double perc = (m_fs_perc + m_pr_params->base_fs_increment) / 100.0;
				value = -m_pr_params->v_switch_default *perc;
			}
			//double value = m_pipette_status->v_switch_set_point -
			//	m_pr_params->v_switch_default * m_pr_params->base_fs_increment / 100.0;
			updateVswitchSetPoint(value);
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			updateVrecircSetPoint(5.0);
		}
		else {
			double value;
			if (m_pr_params->useDefValSetPoint)
			{
				value = m_pipette_status->v_recirc_set_point -
					m_pr_params->v_recirc_default * m_pr_params->base_fs_increment / 100.0;
			}
			else
			{
				double perc = (m_fs_perc + m_pr_params->base_fs_increment) / 100.0;
				value = -m_pr_params->v_recirc_default * perc;
			}
			//double value = m_pipette_status->v_recirc_set_point -
			//	m_pr_params->v_recirc_default * m_pr_params->base_fs_increment / 100.0;
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
		if (m_pr_params->useDefValSetPoint)
		{
			success = m_ppc1->changeFlowspeedBy(-m_pr_params->base_fs_increment);
		}
		else {
			success = m_ppc1->setFlowspeed(m_fs_perc -
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
		
		if (m_fs_perc < MIN_FLOW_SPEED_PERC + m_pr_params->base_fs_increment) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done); 
			return;
		}

		double value;
		if (m_pr_params->useDefValSetPoint)
		{
			value = m_pipette_status->pon_set_point -
				m_pr_params->p_on_default * m_pr_params->base_fs_increment / 100.0;
			updatePonSetPoint(value);
			value = m_pipette_status->poff_set_point -
				m_pr_params->p_off_default * m_pr_params->base_fs_increment / 100.0;
			updatePoffSetPoint(value);
			value = m_pipette_status->v_switch_set_point +
				m_pr_params->v_switch_default * m_pr_params->base_fs_increment / 100.0;
			updateVswitchSetPoint(value);
			value = m_pipette_status->v_recirc_set_point +
				m_pr_params->v_recirc_default * m_pr_params->base_fs_increment / 100.0;
			updateVrecircSetPoint(value);
		}
		else
		{
			double perc = (m_fs_perc - m_pr_params->base_fs_increment) / 100.0;
			value = m_pr_params->p_on_default * perc;
			updatePonSetPoint(value);
			value = m_pr_params->p_off_default * perc;
			updatePoffSetPoint(value);
			value = -m_pr_params->v_switch_default * perc;
			updateVswitchSetPoint(value);
			value = -m_pr_params->v_recirc_default * perc;
			updateVrecircSetPoint(value);
		}

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
		if (m_pr_params->useDefValSetPoint)
		{
			success = m_ppc1->changeVacuumPercentageBy(m_pr_params->base_v_increment);
		}
		else {
			success = m_ppc1->setVacuumPercentage(m_v_perc + 
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

		/*if (m_v_perc > MAX_VACUUM_PERC - m_pr_params->base_v_increment) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done); 
			return;
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			ui->horizontalSlider_recirculation->setValue(5);
		}
		else {

			//double value = m_pipette_status->v_recirc_set_point - m_pr_params->v_recirc_default * m_pr_params->base_v_increment / 100.0;
			//cout << "Labonatip_GUI::vacuumPlus    ::: new recirculation value " << value << endl;
			//updateVrecircSetPoint(value);
			double perc = m_v_perc + m_pr_params->base_v_increment;
			this->setVacuumPercentage(perc);
			
		}*/

		double perc = m_v_perc + m_pr_params->base_v_increment;
		this->setVacuumPercentage(perc);
		updateFlowControlPercentages();
	}
}

void Labonatip_GUI::vacuumMinus() {

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::vacuumMinus    " << endl;

	// -x% v_recirculation
	if (m_pipette_active) {
		bool success = false; 
		if (m_pr_params->useDefValSetPoint)
		{
			success = m_ppc1->changeVacuumPercentageBy(-m_pr_params->base_v_increment);
		}
		else {
			success = m_ppc1->setVacuumPercentage(m_v_perc - //ui->lcdNumber_vacuum_percentage->value() -
				m_pr_params->base_v_increment);
		}

		if (!success) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		}
		else {
			// update the set point
			m_pipette_status->v_recirc_set_point = -m_ppc1->getVrecircSetPoint();
			//ui->label_recircPressure->setText(QString(QString::number(m_v_recirc_set_point) + " mbar"));

			// update the slider for the GUI
			ui->horizontalSlider_recirculation->blockSignals(true);
			ui->horizontalSlider_recirculation->setValue(m_pipette_status->v_recirc_set_point);
			ui->horizontalSlider_recirculation->blockSignals(false);

		}
	}
	if (m_simulationOnly) {

		/*if (m_v_perc < MIN_VACUUM_PERC + m_pr_params->base_v_increment) {
			QMessageBox::information(this, m_str_warning, 
				m_str_operation_cannot_be_done); 
			return;
		}

		if (ui->horizontalSlider_recirculation->value() == 0) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done + "<br>" + m_str_out_of_bound);
		}*/

		//double value = m_pipette_status->v_recirc_set_point +
		//	m_pr_params->v_recirc_default * m_pr_params->base_v_increment / 100.0;
		//cout << "Labonatip_GUI::vacuumMinus    ::: new recirculation value " << value << endl;
		//updateVrecircSetPoint(value);
		double perc = m_v_perc - m_pr_params->base_v_increment;
		this->setVacuumPercentage(perc);

		//updateFlowControlPercentages();
	}
}

void Labonatip_GUI::setDropletSizePercentage(double _perc)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::setDropletSize    " << _perc << endl;

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
		
		delta = (1.0 - std::pow(perc, (1.0 / 3.0)));
		value = m_pr_params->p_on_default - m_pr_params->p_on_default  * delta;

		updatePonSetPoint(value);
		
	}
}

void Labonatip_GUI::setFlowspeedPercentage(double _perc)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::setFlowSpeed    " << _perc << endl;

	if (m_simulationOnly) {
		if (_perc < MIN_FLOW_SPEED_PERC || _perc > MAX_FLOW_SPEED_PERC) {
		//if (m_fs_perc > MAX_FLOW_SPEED_PERC - m_pr_params->base_fs_increment) {
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

void Labonatip_GUI::setVacuumPercentage(double _perc)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::setVacuumPercentage    " << _perc << endl;

	if (m_simulationOnly) {

		if (_perc < MIN_VACUUM_PERC || _perc > MAX_VACUUM_PERC) {
			QMessageBox::information(this, m_str_warning,
				m_str_operation_cannot_be_done);
			return;
		}	

		double value = -m_pr_params->v_recirc_default * _perc/100.0;
		//m_pipette_status->v_recirc_set_point + 
		//m_pr_params->v_recirc_default * m_pr_params->base_v_increment / 100.0;
		cout << "Labonatip_GUI::setVacuumPercentage    ::: new recirculation value "
			<< value << endl;
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
							<< m_fs_perc << " " << endl;
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
		if (m_ds_perc < 0 && m_ds_perc < 1000) {
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
