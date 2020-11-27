/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolTreeWidgetItem.h"
#include <QSpinBox>


protocolTreeWidgetItem::protocolTreeWidgetItem(protocolTreeWidgetItem *_parent) :
	m_pr_params(new pr_params),
	m_cmd_idx_c(0), m_cmd_command_c (1), m_cmd_range_c (2),
	m_cmd_value_c (3), m_cmd_msg_c (4), m_cmd_level_c (5)
{

	//	std::cout << HERE << std::endl;

	QFont font;
	font.setBold(true);

	this->setText(editorParams::c_idx, "0");
	this->setText(editorParams::c_command, QString::number(0));
	this->setText(editorParams::c_range, this->getRangeColumn(0));
	this->setText(editorParams::c_value, ""); // 
	this->QTreeWidgetItem::setFont(editorParams::c_value, font);

	//this->setCheckState(m_cmd_msg_c, Qt::CheckState::Checked); // status message
	this->setText(m_cmd_msg_c, " "); // status message
	this->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));


	m_tt_c_idx = tr("Index");
	m_tt_c_cmd = tr("Protocol command");
	m_tt_c_range = tr("Range of validity");
	m_tt_c_value = tr("Value to be applied");
	m_tt_c_msg = tr("Write here any message here");
	m_tt_cmd_alloff = tr("Switch off all pressure pumps and close all solution valves");
	m_tt_cmd_solution = tr("Open/close the solution X valve");
	m_tt_cmd_setPon = tr("Set the pressureON channel to a specific value");
	m_tt_cmd_setPoff = tr("Set the pressureOFF channel to a specific value");
	m_tt_cmd_setVrecirc = tr("Set the vacuum recirculation channel to a specific value");
	m_tt_cmd_setVswitch = tr("Set the vacuum switch channel to a specific value");
	m_tt_cmd_waitSync = tr("");
	m_tt_cmd_syncOut = tr("");
	m_tt_cmd_wait = tr("Set a waiting time in seconds");
	m_tt_cmd_ask = tr("Stop the protocol and ask a confirmation message");
	m_tt_cmd_pumpsOff = tr("Switch off all the pressure pumps");
	m_tt_cmd_loop = tr("Create a loop of commands");
	m_tt_cmd_comment = tr("Comment for the designer, no effect on the protocol running");
	m_tt_cmd_button = tr("Run push/stop protocol for button X");
	m_tt_cmd_ramp = tr("");
	m_tt_cmd_operational = tr("Run the operational protocol");
	m_tt_cmd_initialize = tr("Run initialize protocol");
	m_tt_cmd_standby = tr("Run standby protocol");
	m_tt_cmd_function = tr("");
	m_tt_cmd_smallAndSlow = tr("Set the droplet to be small and flow to be slow");
	m_tt_cmd_smallAndFast = tr("Set the droplet to be small and flow to be fast");
	m_tt_cmd_bigAndSlow = tr("Set the droplet to be big and flow to be slow");
	m_tt_cmd_bigAndFast = tr("Set the droplet to be big and flow to be fast");

	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_idx, m_tt_c_idx);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_c_cmd);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_range, m_tt_c_range);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_value, m_tt_c_value);
	dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_msg, m_tt_c_msg);

	if (_parent)
	{
		this->setParent(_parent);
	}
}


bool protocolTreeWidgetItem::checkValidity( int _column)
{
	// check validity for the element

	if (this->childCount() > 0)
	{
		// If we have children than the item IS a loop
		// so we force the item column to the loop function 

		//this->setText(m_cmd_command_c, QString::number(ppc1Cmd::loop));

		// 16 is loop and 28 is function
		std::string ss = this->text(editorParams::c_command).toStdString();
		if (ss != "16" && ss != "28")
		{
#pragma message (" TODO: check validity for loop ")
			this->setText(editorParams::c_command, QString::number(pCmd::loop));
		}
	}

	// get the command index 
	int idx = this->text(editorParams::c_command).toInt();
    
	// if the change comes from the command column
	// the range field needs to be reset
	//TODO: this was deprecated, remove if everything works
	//if (_column == m_cmd_command_c) {
	//	this->blockSignals(true);
	//	this->setText(m_cmd_range_c, this->getRangeColumn(idx));
	//	this->blockSignals(false);
		// so it also automatically check the other column
	//	_column = m_cmd_value_c;
	//}


	// TODO: this can also be removed when the old protocol version will be forgotten
	//check for prohibited characters  # and §
	if (_column == editorParams::c_msg) {
		// here we browse the string looking for prohibited characters  # and §
		QString s = this->text(_column);
		QChar prohibited_char_1 = QChar::fromLatin1(*"#");
		QChar prohibited_char_2 = QChar::fromLatin1(*"\n");
		if (s.contains(prohibited_char_1, Qt::CaseSensitive) ||
			s.contains(prohibited_char_2, Qt::CaseSensitive))
		{
			s.remove(prohibited_char_1, Qt::CaseSensitive);
			s.remove(prohibited_char_2, Qt::CaseSensitive);
			// we need to remove the string
			this->setText(_column, s);
			return true;
		}
	}


	// perform the check on column 3 only
	//if (_column != editorParams::c_value) return true;
	// get the number to be checked
	int number = this->text(editorParams::c_value).toInt();
	switch (idx)
	{
	case protocolCommands::setZoneSize: {
		// Droplet size (%) 
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < MIN_ZONE_SIZE_PERC ||
			number > MAX_ZONE_SIZE_PERC) { // if is not the range
			this->setText(_column, QString("100")); // if the value is not valid, reset to 100 %
			return false;
		}
		break;

	}
	case protocolCommands::changeZoneSizeBy: {
		// Vacuum (%) 
		int number = this->text(_column).toInt();
		if (std::abs(number) > MAX_ZONE_SIZE_INCREMENT) {  // if is not the range
			this->setText(_column, QString("0")); // if the value is not valid, reset to 0 %
			return false;
		}
		break;

	}
	case protocolCommands::setFlowSpeed: {
		// Flow speed (%)
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < MIN_FLOW_SPEED_PERC ||
			number > MAX_FLOW_SPEED_PERC) { // if is not the range
			this->setText(_column, QString("100")); // if the value is not valid, reset to 100 %
			return false;
		}
		break;

	}
	case protocolCommands::changeFlowSpeedBy: {
		// Vacuum (%) 
		int number = this->text(_column).toInt();
		if (std::abs(number) > MAX_FLOW_SPEED_INCREMENT) { // if is not the range
			this->setText(_column, QString("0")); // if the value is not valid, reset to 0 %
			return false;
		}
		break;

	}
	case protocolCommands::setVacuum: {
		// Vacuum (%) 
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < MIN_VACUUM_PERC ||
			number > MAX_VACUUM_PERC) { // if is not the range
			this->setText(_column, QString("100")); // if the value is not valid, reset to 100 %
			return false;
		}
		break;

	}
	case protocolCommands::changeVacuumBy: {
		// Vacuum (%) 
		int number = this->text(_column).toInt();
		if (std::abs(number) > MAX_VACUUM_INCREMENT) { // if is not the range
			this->setText(_column, QString("0")); // if the value is not valid, reset to 0 %
			return false;
		}
		break;
	}

	case protocolCommands::allOff: {
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_alloff);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::solution1:
	case protocolCommands::solution2:
	case protocolCommands::solution3:
	case protocolCommands::solution4: {
		// check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_solution);
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setPon: {
		// if the number if lower than 0,
		// the value becomes automatically positive
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setPon);
		if (number < 1) {
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (//number < m_pr_params->p_off_min || // not necessary as it min=0
			number > m_pr_params->p_on_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->p_on_max));
			return false;
		}
		return true;
	}
	case protocolCommands::setPoff: {
		// if the number if lower than 0,
		// the value becomes automatically positive
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setPoff);
		if (number < 1)
		{
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (//number < m_pr_params->p_off_min || // not necessary as it min=0
			number > m_pr_params->p_off_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->p_off_max)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setVrecirc: {
		// if the number if higher than 0,
		// the value becomes automatically negative
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setVrecirc);
		if (number > -1)
		{
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (number < m_pr_params->v_recirc_min) {  
			// || number > m_pr_params->v_recirc_max) { // not necessary as it max=0
			this->setText(editorParams::c_value, QString::number(m_pr_params->v_recirc_min)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setVswitch: {
		// if the number if higher than 0,
		// the value becomes automatically negative
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_setVswitch);
		if (number > -1) {
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (number < m_pr_params->v_switch_min){
			//number > m_pr_params->v_switch_max) {  // not necessary as it max=0
			this->setText(editorParams::c_value, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::waitSync: {
		// if the value is not valid, reset to zero
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_waitSync);
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0"));
			return false;
		}
		return true;
	}
	case protocolCommands::syncOut: {
		// if the value is not valid, reset to zero
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_syncOut);
		if (number < MIN_PULSE_PERIOD) { // if is not the range
			this->setText(editorParams::c_value, QString("20"));
			return false;
		}
		return true;
	}
	case protocolCommands::wait: {
		// if the value is not valid, reset to one
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_wait);
		if (number < 1) {
			this->setText(editorParams::c_value, QString("1"));
			return false;
		}
		return true;
	}
	case protocolCommands::ask: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_ask);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::pumpsOff: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_pumpsOff);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::loop: {
		// if is not the range minimum number is 1
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_loop);
		if (number < 1) {
			this->setText(editorParams::c_value, QString("1")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case protocolCommands::comment: {
		// no need to check here
		dynamic_cast<QTreeWidgetItem*>(this)->setToolTip(editorParams::c_command, m_tt_cmd_comment);
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	default: {
		// default function active if none of the previous

		break;
		}
	}
	
	return true;
}

QString protocolTreeWidgetItem::getRangeColumn( int _idx)
{

#pragma message (" TODO: getRangeColumn ")
	ComboBoxDelegate cb;

	switch (_idx) {
	case protocolCommands::allOff://pCmd::setPoff: 
	{
		return QString("-");
	}
	case pCmd::setZoneSize: {
		// Droplet size (%) -- "(%) [50, 200]"
		return QString("(%) [" + QString::number(MIN_ZONE_SIZE_PERC) +
			", " + QString::number(MAX_ZONE_SIZE_PERC) + "]");
	}
	case pCmd::setFlowSpeed: {
		// Flow speed (%) -- "(%) [50, 220]"
		return QString("(%) [" + QString::number(MIN_FLOW_SPEED_PERC) +
			", " + QString::number(MAX_FLOW_SPEED_PERC) + "]");
	}
	case pCmd::setVacuum: {
		// Vacuum (%)   -- "(%) [50, 250]"
		return QString("(%) [" + QString::number(MIN_VACUUM_PERC) +
			", " + QString::number(MAX_VACUUM_PERC) + "]");
	}
	case pCmd::changeZoneSizeBy: {
		// Droplet size (%)  -- "(%) [-40, 40]"
		return QString("(%) [-" + QString::number(MAX_ZONE_SIZE_INCREMENT) +
			", " + QString::number(MAX_ZONE_SIZE_INCREMENT) + "]");
	}
	case pCmd::changeFlowSpeedBy: {
		// Flow speed (%) -- "(%) [-40, 40]"
		return QString("(%) [-" + QString::number(MAX_FLOW_SPEED_INCREMENT) +
			", " + QString::number(MAX_FLOW_SPEED_INCREMENT) + "]");
	}
	case pCmd::changeVacuumBy: {
		// Vacuum (%) -- "(%) [-40, 40]"
		return QString("(%) [-" + QString::number(MAX_VACUUM_INCREMENT) +
			", " + QString::number(MAX_VACUUM_INCREMENT) + "]");
	}
	case protocolCommands::solution1://pCmd::solution1: 
	case protocolCommands::solution2://pCmd::solution2:
	case protocolCommands::solution3://pCmd::solution3:
	case protocolCommands::solution4://pCmd::solution4:
	{ 
		// check open valve : 0 = no valve, 1,2,3,4,5,6 valves 1,2,3,4,5,6
		return QString("1/0 open/close");
	} 
	case protocolCommands::setPon://pCmd::setPon:
	case protocolCommands::setPoff://pCmd::setPoff:
	{ // check pressures
		return QString("(mbar) [" + QString::number(MIN_CHAN_C) +
			", " + QString::number(MAX_CHAN_C) + "] ");
	}
	case protocolCommands::setVrecirc:	
	case protocolCommands::setVswitch: { // check vacuums
		return QString("(mbar) [" + QString::number(MIN_CHAN_A) +
			", " + QString::number(MAX_CHAN_A) + "]");
	}
	case protocolCommands::waitSync: {
		// Wait sync"
		return QString("1/0 rise/fall");
	}
	case protocolCommands::syncOut: {
		// Sync out"
		return QString(">20");
	}
	case protocolCommands::wait: {
		// check Wait (s)
		return QString("(s) > 0");
	}
	case protocolCommands::ask: {
		// ask 	
		return QString("-");
	}
	case protocolCommands::pumpsOff: {
		// pumps off
		return QString("-");
	}
	case protocolCommands::loop: {
		// loop	
		return QString(" > 0");
	}
	case protocolCommands::comment: {
		// Comment	
		return QString("-");
	}
	default: {
		// default function active if none of the previous
		return QString("Unknown");
	}
	}

}

void protocolTreeWidgetItem::setElements(int _cmd_ind, int _value, bool _show_msg, QString _msg)
{
	this->blockSignals(true);
	this->setText(m_cmd_idx_c, "0");
	this->setText(m_cmd_command_c, QString::number(_cmd_ind));
	this->setText(m_cmd_range_c, this->getRangeColumn(_cmd_ind));
	this->setText(m_cmd_value_c, QString::number(_value)); 
	//this->setCheckState(m_cmd_msg_c, Qt::CheckState(_show_msg)); // status message
	this->setText(m_cmd_msg_c, _msg); // status message
	this->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));
	this->blockSignals(false);
}

void protocolTreeWidgetItem::setData(int column, int role, const QVariant & value)
{
	//cout << HERE 
	//	<< " previous " << this->text(column).toStdString() 
	//	<< " new value " << value.toString().toStdString() << endl;

	// this is to keep tracks of changes in the data items, 
	// it is then possible to overload a signal and 
	// bring the last value to the undo stack
	m_last_command = this->text(1).toInt();
	m_last_value = this->text(3).toInt();
	m_last_msg = this->text(4);

	this->QTreeWidgetItem::setData(column, role, value);

}

protocolTreeWidgetItem * protocolTreeWidgetItem::clone()
{
		protocolTreeWidgetItem * clone = new protocolTreeWidgetItem();

		QFont font;
		font.setBold(true);

		clone->setText (m_cmd_idx_c, this->text(m_cmd_idx_c));
		clone->setText (m_cmd_command_c, this->text(m_cmd_command_c));
		clone->setText(m_cmd_range_c, this->text(m_cmd_range_c));
		clone->setText(m_cmd_value_c, this->text(m_cmd_value_c)); // 
		clone->QTreeWidgetItem::setFont(m_cmd_value_c, font);

		//clone->setCheckState(m_cmd_msg_c, this->checkState(m_cmd_msg_c)); // status message
		clone->setText(m_cmd_msg_c, this->text(m_cmd_msg_c)); // status message
		clone->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));

		return clone;
}

