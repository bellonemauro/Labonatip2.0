/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
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

	//cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "protocolTreeWidgetItem::protocolTreeWidgetItem   " << endl;

	QFont font;
	font.setBold(true);

	this->setText(m_cmd_idx_c, "0");
	this->setText(m_cmd_command_c, QString::number(0));
	this->setText(m_cmd_range_c, this->getRangeColumn(0));
	this->setText(m_cmd_value_c, "1"); // 
	this->QTreeWidgetItem::setFont(m_cmd_value_c, font);

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
		this->setText(m_cmd_command_c, QString::number(17));
	}

	// get the command index 
	int idx = this->text(m_cmd_command_c).toInt();

	// if the change comes from the command column
	// the range field needs to be reset
	if (_column == m_cmd_command_c) {
		this->blockSignals(true);
		this->setText(m_cmd_range_c, this->getRangeColumn(idx));
		this->blockSignals(false);
		// so it also automatically check the other column
		_column = m_cmd_value_c;
	}

	//check for prohibited characters  # and §
	if (_column == m_cmd_msg_c) {
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
	if (_column != m_cmd_value_c) return true;

	switch (idx) {
	case fluicell::PPC1api::command::instructions::setPon: { // check pon

	    // get the number to be checked
		int number = this->text(_column).toInt();

		// if the number if lower than 0,
		// the value becomes automatically positive
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (//number < m_pr_params->p_on_min || not necessary anymore
			number > m_pr_params->p_on_max) { // if is not the range

											  // values higher than the limit are not allowed
			this->setText(_column, QString::number(m_pr_params->p_on_max));
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::setPoff: { // check poff

			  // get the number to be checked
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < m_pr_params->p_off_min ||
			number > m_pr_params->p_off_max) { // if is not the range

			this->setText(_column, QString::number(m_pr_params->p_off_max)); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::setVswitch: {// check v_s

		int number = this->text(_column).toInt();
		if (number > 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < m_pr_params->v_switch_min ||
			number > m_pr_params->v_switch_max) { // if is not the range
			this->setText(_column, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::setVrecirc: { // check v_r

		int number = this->text(_column).toInt();
		if (number > 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < m_pr_params->v_recirc_min ||  
			number > m_pr_params->v_recirc_max) { // if is not the range
			this->setText(_column, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::solution1: 
	case fluicell::PPC1api::command::instructions::solution2: 
	case fluicell::PPC1api::command::instructions::solution3: 
	case fluicell::PPC1api::command::instructions::solution4: 
	{ //from 4 to 7
	// check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4

		int number = this->text(_column).toInt();
		if (number != 0 &&
			number != 1) {
			this->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::wait: {
		// check Wait (s)

		int number = this->text(_column).toInt();

		if (number < 1) { // if is not the range
			this->setText(_column, QString("1")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::ask_msg: {
		// ask 
		// no need to check here

		this->setText(_column, QString("")); // it removes whatever is there
		break;
	}
	case fluicell::PPC1api::command::instructions::allOff: {
		// all off
		// no need to check here
		this->setText(_column, QString("")); // it removes whatever is there
		break;
	}
	case fluicell::PPC1api::command::instructions::pumpsOff: {
		// pumps off
		// no need to check here
		this->setText(_column, QString("")); // it removes whatever is there
		break;
	}
	case fluicell::PPC1api::command::instructions::waitSync: {
		// Wait sync"
		int number = this->text(_column).toInt();

		if (number != 0 &&
			number != 1) {
			this->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::syncOut: {
		// Sync out"
		int number = this->text(_column).toInt();
		if (number < MIN_PULSE_PERIOD) { // if is not the range
			this->setText(_column, QString("20")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::zoneSize: {
		// Droplet size (%) //TODO: remove this !!!

		//qobject_cast<QComboBox*>(ui_p_editor->treeWidget_macroTable->itemWidget(this, m_cmd_command_c))->setCurrentIndex(0);
		//QMessageBox::warning(this, m_str_warning,
		//	m_str_check_validity_msg11);
		//this->setText(_column, "0"); 
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < 50 || //TODO
			number > 200) { // if is not the range
			this->setText(_column, QString("100")); // if the value is not valid, reset to 100 %
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::flowSpeed: {
		// Flow speed (%)
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < 50 || //TODO
			number > 250) { // if is not the range
			this->setText(_column, QString("100")); // if the value is not valid, reset to 100 %
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::vacuum: {
		// Vacuum (%) //TODO : remove this 
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < 50 || //TODO
			number > 250) { // if is not the range
			this->setText(_column, QString("100")); // if the value is not valid, reset to 100 %
			return false;
		}
		break;
	}
	case fluicell::PPC1api::command::instructions::loop: {   //TODO: TO BE REMOVED
				 // check loops

		int number = this->text(_column).toInt();
		if (number < 1) { // if is not the range
			this->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
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

	switch (_idx) {
	case fluicell::PPC1api::command::instructions::setPoff: 
	case fluicell::PPC1api::command::instructions::setPon: { // check pressures
		return QString("(mbar) [0, 450]");
	}
	case fluicell::PPC1api::command::instructions::setVswitch:	
	case fluicell::PPC1api::command::instructions::setVrecirc: { // check vacuums
		return QString("(mbar) [-300, 0]");
	}
	case fluicell::PPC1api::command::instructions::solution1:
	case fluicell::PPC1api::command::instructions::solution2:
	case fluicell::PPC1api::command::instructions::solution3:
	case fluicell::PPC1api::command::instructions::solution4: 
	{ //from 4 to 7
									  // check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
		return QString("1/0 open/close");
	}
	case fluicell::PPC1api::command::instructions::wait: {
		// check Wait (s)
		return QString("(s) > 0");
	}
	case fluicell::PPC1api::command::instructions::ask_msg: {
		// ask 	
		return QString("-");
	}
	case fluicell::PPC1api::command::instructions::allOff: {
		// all off	
		return QString("-");
	}
	case fluicell::PPC1api::command::instructions::pumpsOff: {
		// pumps off
		return QString("-");
	}
	case fluicell::PPC1api::command::instructions::waitSync: {
		// Wait sync"
		return QString("1/0 rise/fall");
	}
	case fluicell::PPC1api::command::instructions::syncOut: {
		// Sync out"
		return QString(">20");
	}
	case fluicell::PPC1api::command::instructions::zoneSize: {
		// Droplet size (%) //TODO: remove this !!!
		return QString("(%) [50, 200]");
	}
	case fluicell::PPC1api::command::instructions::flowSpeed: {
		// Flow speed (%)
		return QString("(%) [50, 250]");
	}
	case fluicell::PPC1api::command::instructions::vacuum: {
		// Vacuum (%) //TODO : remove this 
		return QString("(%) [50, 250]");
	}
	case fluicell::PPC1api::command::instructions::loop: {
		// check loops
		return QString("(#) > 0");
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


	//cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "protocolTreeWidgetItem::setData   " 
	//	<< " previous " << this->text(column).toStdString() 
	//	<< " new value " << value.toString().toStdString() << endl;

	// this is to keep tracks of changes in the data items, 
	// it is then possible to overload a signal and 
	// bring the last value to the undo stack
	m_last_command = this->text(1).toInt();
	m_last_value = this->text(3).toInt();
	//m_last_show_msg = this->checkState(4);
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

