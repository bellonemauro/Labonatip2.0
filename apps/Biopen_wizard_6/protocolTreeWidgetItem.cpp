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
			this->setText(editorParams::c_command, QString::number(ppc1Cmd::loop));
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
	case protocolCommands::allOff: {
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::solution1:
	case protocolCommands::solution2:
	case protocolCommands::solution3:
	case protocolCommands::solution4:
	case protocolCommands::solution5:
	case protocolCommands::solution6: {
		// check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::setPon: {
		// if the number if lower than 0,
		// the value becomes automatically positive
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
		int number = this->text(editorParams::c_value).toInt();
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
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0"));
			return false;
		}
		return true;
	}
	case protocolCommands::syncOut: {
		// if the value is not valid, reset to zero
		if (number < MIN_PULSE_PERIOD) { // if is not the range
			this->setText(editorParams::c_value, QString("20"));
			return false;
		}
		return true;
	}
	case protocolCommands::wait: {
		// if the value is not valid, reset to one
		if (number < 1) {
			this->setText(editorParams::c_value, QString("1"));
			return false;
		}
		return true;
	}
	case protocolCommands::ask: {
		// no need to check here
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::pumpsOff: {
		// no need to check here
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::loop: {
		// if is not the range minimum number is 1
		if (number < 1) {
			this->setText(editorParams::c_value, QString("1")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case protocolCommands::comment: {
		// no need to check here
		this->setText(editorParams::c_value, QString("")); // it removes whatever is there
		return true;
	}
	case protocolCommands::button1:
	case protocolCommands::button2:
	case protocolCommands::button3:
	case protocolCommands::button4:
	case protocolCommands::button5:
	case protocolCommands::button6: {
		// check pump button : 0 = stop, 1 = pump
		// if the value is not valid, reset to zero
		if (number != 1) {
			this->setText(editorParams::c_value, QString("0"));
			return false;
		}
		return true;
	}
	case protocolCommands::rampPon: {
		// if the number if lower than 0,
		// the value becomes automatically positive
		if (number < 1) {
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (//number < m_pr_params->p_off_min ||
			number > m_pr_params->p_on_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->p_on_max));
			return false;
		}
		return true;
	}
	case protocolCommands::rampPoff: {
		// if the number if lower than 0,
		// the value becomes automatically positive
		if (number < 1)
		{
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (//number < m_pr_params->p_off_min ||
			number > m_pr_params->p_off_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->p_off_max)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::rampVr: {
		// if the number if higher than 0,
		// the value becomes automatically negative
		int number = this->text(editorParams::c_value).toInt();
		if (number > -1)
		{
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (number < m_pr_params->v_recirc_min){// ||
			//number > m_pr_params->v_recirc_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->v_recirc_min)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::rampVs: {
		// if the number if higher than 0,
		// the value becomes automatically negative
		if (number > -1) {
			number = -number;
			this->setText(editorParams::c_value, QString::number(number));
		}
		// if is not the range
		if (number < m_pr_params->v_switch_min){// ||
			//number > m_pr_params->v_switch_max) {
			this->setText(editorParams::c_value, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		return true;
	}
	case protocolCommands::function: {
		// no need to check here
		this->setText(editorParams::c_value, QString("1")); // it can only be 1
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
	case protocolCommands::solution1://pCmd::solution1: 
	case protocolCommands::solution2://pCmd::solution2:
	case protocolCommands::solution3://pCmd::solution3:
	case protocolCommands::solution4://pCmd::solution4:
	case protocolCommands::solution5://pCmd::solution5:
	case protocolCommands::solution6://pCmd::solution6:
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
	case protocolCommands::button1: // Button1
	case protocolCommands::button2: // Button2
	case protocolCommands::button3: // Button3
	case protocolCommands::button4: // Button4
	case protocolCommands::button5: // Button5
	case protocolCommands::button6: {// Button6	
		return QString("1/0 pump/stop");
	}
	case protocolCommands::rampPon:	// RampPon	
	case protocolCommands::rampPoff: {
		// RampPoff	
		return QString("(mbar) [" + QString::number(MIN_CHAN_C) +
			", " + QString::number(MAX_CHAN_C) + "] ");
	}
	case protocolCommands::rampVr: // RampPVr	
	case protocolCommands::rampVs: {
		// RampVs	
		return QString("(mbar) [" + QString::number(MIN_CHAN_A) +
			", " + QString::number(MAX_CHAN_A) + "]");
	}
#pragma message (" TODO: here instructions were removed setFlowSpeed, setVacuum and setSize, others should be added")
	case protocolCommands::function: {
		// function
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

