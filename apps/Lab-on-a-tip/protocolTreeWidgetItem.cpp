/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
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
	//	<< "myQTreeWidgetItem::myQTreeWidgetItem   " << endl;

	QFont font;
	font.setBold(true);

	this->setText(m_cmd_idx_c, "0");
	this->setText(m_cmd_command_c, QString::number(0));
	this->setText(m_cmd_range_c, this->getRangeColumn(0));
	this->setText(m_cmd_value_c, "1"); // 
	this->QTreeWidgetItem::setFont(m_cmd_value_c, font);

	this->setCheckState(m_cmd_msg_c, Qt::CheckState::Unchecked); // status message
	this->setText(m_cmd_msg_c, " "); // status message
	this->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));

	if (_parent)
	{
		this->setParent(_parent);
	}

}


bool protocolTreeWidgetItem::checkValidity( int _column) // TODO: _column is not used
{
	// check validity for the element

	if (this->childCount() > 0)
	{
		// TODO: check here, if we have children than the item IS a loop
		this->setText(m_cmd_command_c, QString::number(17));
	}

	int idx = this->text(m_cmd_command_c).toInt();
	if (_column == m_cmd_command_c) {
		this->setText(m_cmd_range_c, this->getRangeColumn(idx));
		// so it also automatically check the other column
		_column = m_cmd_value_c;
	}

	// perform the check on column 3 only
	if (_column != m_cmd_value_c) return true;

	switch (idx) {
	case 0: { // check pon

			  // get the number to be checked
		int number = this->text(_column).toInt();

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
	case 1: { // check poff

			  // get the number to be checked
		int number = this->text(_column).toInt();
		if (number < 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < m_pr_params->p_off_min ||//TODO fix this conditions
			number > m_pr_params->p_off_max) { // if is not the range

			this->setText(_column, QString::number(m_pr_params->p_off_max)); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 2: {// check v_s

		int number = this->text(_column).toInt();
		if (number > 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < m_pr_params->v_switch_min ||//TODO fix this conditions
			number > m_pr_params->v_switch_max) { // if is not the range
			this->setText(_column, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 3: { // check v_r

		int number = this->text(_column).toInt();
		if (number > 0)
		{
			number = -number;
			this->setText(_column, QString::number(number));
		}
		if (number < m_pr_params->v_recirc_min ||  //TODO fix this conditions
			number > m_pr_params->v_recirc_max) { // if is not the range
			this->setText(_column, QString::number(m_pr_params->v_switch_min)); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 4: case 5: case 6: case 7: { //from 4 to 7
									  // check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4

		int number = this->text(_column).toInt();
		if (number != 0 &&
			number != 1) {
			this->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 8: {
		// check Wait (s)

		int number = this->text(_column).toInt();

		if (number < 1) { // if is not the range
			this->setText(_column, QString("1")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 9: {
		// ask 
		// no need to check here

		this->setText(_column, QString("")); // it removes whatever is there
		break;
	}
	case 10: {
		// all off
		// no need to check here
		this->setText(_column, QString("")); // it removes whatever is there
		break;
	}
	case 11: {
		// pumps off
		// no need to check here
		this->setText(_column, QString("")); // it removes whatever is there
		break;
	}
	case 12: {
		// Wait sync"
		int number = this->text(_column).toInt();

		if (number != 0 &&
			number != 1) {
			this->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 13: {
		// Sync out"
		int number = this->text(_column).toInt();
		if (number < MIN_PULSE_PERIOD) { // if is not the range
			this->setText(_column, QString("20")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 14: {
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
	case 15: {
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
	case 16: {
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
	case 17: {   //TODO: TO BE REMOVED
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
	case 0: case 1: { // check pressures
		return QString("(mbar) [0, 450]");
	}
	case 2:	case 3: { // check vacuums
		return QString("(mbar) [-300, 0]");
	}
	case 4: case 5: case 6: case 7: { //from 4 to 7
									  // check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
		return QString("1/0 open/close");
	}
	case 8: {
		// check Wait (s)
		return QString("(s) > 0");
	}
	case 9: {
		// ask 	
		return QString("-");
	}
	case 10: {
		// all off	
		return QString("-");
	}
	case 11: {
		// pumps off
		return QString("-");
	}
	case 12: {
		// Wait sync"
		return QString("1/0 rise/fall");
	}
	case 13: {
		// Sync out"
		return QString(">20");
	}
	case 14: {
		// Droplet size (%) //TODO: remove this !!!
		return QString("(%) [50, 200]");
	}
	case 15: {
		// Flow speed (%)
		return QString("(%) [50, 250]");
	}
	case 16: {
		// Vacuum (%) //TODO : remove this 
		return QString("(%) [50, 250]");
	}
	case 17: {
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

	this->setText(m_cmd_idx_c, "0");
	this->setText(m_cmd_command_c, QString::number(_cmd_ind));
	this->setText(m_cmd_range_c, this->getRangeColumn(_cmd_ind));
	this->setText(m_cmd_value_c, QString::number(_value)); 
	this->setCheckState(m_cmd_msg_c, Qt::CheckState(_show_msg)); // status message
	this->setText(m_cmd_msg_c, _msg); // status message
	this->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));
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

		clone->setCheckState(m_cmd_msg_c, this->checkState(m_cmd_msg_c)); // status message
		clone->setText(m_cmd_msg_c, this->text(m_cmd_msg_c)); // status message
		clone->setFlags(this->flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));


		return clone;
}
