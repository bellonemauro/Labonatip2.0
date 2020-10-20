/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolCommands.h"

addProtocolCommand::addProtocolCommand(
	QTreeWidget * const _tree_widget, int _at_row, 
	protocolTreeWidgetItem * _parent)
	: m_tree_widget(_tree_widget),
	m_at_row(_at_row)
{
	
	is_undo = 0;
	m_has_parent = false;
	m_parent_row = 0;
	m_parent = NULL;

	// if we have a parent we save it into a member
	if (_parent)
	{
		m_has_parent = true;
		m_parent_row = _tree_widget->indexOfTopLevelItem(_parent);
		m_parent = _parent;
	}

}

void addProtocolCommand::redo()
{
	//	std::cout << HERE << std::endl;
	
	// create the new command to add
	m_new_item = new protocolTreeWidgetItem();

	// if this is the redo running after and undo
	// the object needs to be re-created as the 
	// undo delete the pointer, so now the object
	// m_new_item is pointing to another location
	if (is_undo == 1)
	{
		// if has a parent, add as a child
		if (m_has_parent)
		{// re-create the item as it was 
			m_new_item->setElements(
				m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
			
			// this is for safety, it is better to update the pointer 
			// to the parent as it could have been deleted during the
			// tree operations
			m_parent = dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_parent_row));
			m_parent->insertChild(m_at_row, m_new_item);
		}
		else
		{// re-create the item as it was 
			m_new_item->setElements(
				m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
			m_tree_widget->insertTopLevelItem(m_at_row, m_new_item);
		}
		is_undo = 0;
	}

	// if we have a parent, the new item is added as a child
	if (m_has_parent)
	{
		m_parent = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_parent_row));
		m_parent->insertChild(m_at_row, m_new_item);
		int rr = m_tree_widget->indexOfTopLevelItem(m_parent);
		this->setText("Added child at " + QString::number(m_at_row)
			+ " parent row " + QString::number(rr));
	}
	// else the new item goes at a specific row in the new level
	else
	{

		m_tree_widget->insertTopLevelItem(m_at_row, m_new_item);

		// text of the command to add in the stack
		this->setText("Added top level item at "
			+ QString::number(m_at_row));
	}
}

void addProtocolCommand::undo()
{
	//	std::cout << HERE << std::endl;

	if (m_tree_widget->topLevelItem(m_at_row) == NULL)
	{
		int i = 0;
	}

	if ( m_has_parent )
	{
		m_parent = m_parent = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_parent_row));

		// get the item to be removed
		m_new_item = dynamic_cast<protocolTreeWidgetItem *> (
			m_parent->child(m_at_row));

		//save the params 
		m_view_idx = m_new_item->text(editorParams::c_idx).toInt();
		m_cmd_idx = m_new_item->text(editorParams::c_command).toInt();
		m_value = m_new_item->text(editorParams::c_value).toInt();
		m_show_status_msg = m_new_item->checkState(editorParams::c_msg);
		m_status_msg = m_new_item->text(editorParams::c_msg);

		// remove the item
		m_parent->takeChild(m_at_row);
	}
	else {

		// get the item to be removed
		m_new_item = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_at_row));

		//save the params 
		m_view_idx = m_new_item->text(editorParams::c_idx).toInt();
		m_cmd_idx = m_new_item->text(editorParams::c_command).toInt();
		m_value = m_new_item->text(editorParams::c_value).toInt();
		m_show_status_msg = m_new_item->checkState(editorParams::c_msg);
		m_status_msg = m_new_item->text(editorParams::c_msg);

		// remove the item
		m_tree_widget->model()->removeRow(m_at_row);
	}
	is_undo = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

removeProtocolCommand::removeProtocolCommand(
	QTreeWidget * const _tree_widget, int _at_row,
	protocolTreeWidgetItem * _parent) :
	m_tree_widget(_tree_widget),
	m_at_row(_at_row)
{
	m_has_parent = false;
	m_parent = NULL;

	if (_parent)
	{
		m_has_parent = true;
		m_parent_row = _tree_widget->indexOfTopLevelItem(_parent);
		m_parent = _parent;
	}
}


void removeProtocolCommand::redo()
{
	//	std::cout << HERE << std::endl;

	// if has a parent we have to remove the child
	if (m_has_parent)
	{
		// update the pointers to parent and item to remove
		m_parent = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_parent_row));
		
		m_remove_item = dynamic_cast<protocolTreeWidgetItem *>(
			m_parent->child(m_at_row));

		// save params
		m_view_idx = m_remove_item->text(editorParams::c_idx).toInt();
		m_cmd_idx = m_remove_item->text(editorParams::c_command).toInt();
		m_value = m_remove_item->text(editorParams::c_value).toInt();
		m_show_status_msg = m_remove_item->checkState(editorParams::c_msg);
		m_status_msg = m_remove_item->text(editorParams::c_msg);

		// remove the child 
		m_parent->takeChild(m_at_row);

		//int rr = m_tree_widget->indexOfTopLevelItem(m_parent);
		this->setText("Removed child at " + QString::number(m_at_row)
			+ " parent row " + QString::number(m_parent_row));

	}
	//else we remove the top level item at row
	else
	{
		//update the pointer
		m_remove_item = dynamic_cast<protocolTreeWidgetItem *>(
			m_tree_widget->topLevelItem(m_at_row) );

		// save params
		m_view_idx = m_remove_item->text(editorParams::c_idx).toInt();
		m_cmd_idx = m_remove_item->text(editorParams::c_command).toInt();
		m_value = m_remove_item->text(editorParams::c_value).toInt();
		m_show_status_msg = m_remove_item->checkState(editorParams::c_msg);
		m_status_msg = m_remove_item->text(editorParams::c_msg);

		// remove the item
		m_tree_widget->model()->removeRow(m_at_row);
		this->setText("Removed top level item at " + QString::number(m_at_row));
	}	
}

void removeProtocolCommand::undo()
{
	//	std::cout << HERE << std::endl;

	// create the item
	m_remove_item = new protocolTreeWidgetItem();

	// if has parent we remove the child
	if (m_has_parent)
	{
		// re-create the item as it was from the stored values 
		m_remove_item->setElements(
			m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
		
		// update the pointer to the parent 
		m_parent = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_parent_row));

		// insert the element as a child at row
		m_parent->insertChild(m_at_row, m_remove_item);
	}
	// else we add it as a top level item
	else
	{
		// re-create the item as it was 
		m_remove_item->setElements(
			m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
		
		// insert the previously removed row as a top level item
		m_tree_widget->insertTopLevelItem(m_at_row, m_remove_item);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

changedProtocolCommand::changedProtocolCommand(
	QTreeWidget * const _tree_widget,
	protocolTreeWidgetItem * _changed_item,
	int _column,
	protocolTreeWidgetItem * _parent) :
	m_tree_widget(_tree_widget),
	m_changed_column (_column)
{
	is_undo = 0;
	m_parent = NULL;
	m_has_parent = false;

	if (_parent)
	{
		m_parent = _parent;
		m_has_parent = true;
		m_parent_row = _tree_widget->indexOfTopLevelItem(_parent);

		m_changed_row = m_parent->indexOfChild(_changed_item);
	}
	else
	{
		m_changed_row = _tree_widget->indexOfTopLevelItem(_changed_item);
	}

	m_changed_item = _changed_item;
}


void changedProtocolCommand::redo()
{
	//	std::cout << HERE << std::endl;

	// if it was previously un-done
	if (is_undo == 1)
	{

		if (m_has_parent)
		{
			// get the parent
			m_parent = dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_parent_row));
			// get the child
			m_changed_item = dynamic_cast<protocolTreeWidgetItem *> (
				m_parent->child(m_changed_row));

			// set the elements from the stored values
			m_changed_item->setElements(m_new_command, m_new_value,
				m_new_show_msg, m_new_msg);
		}
		// update the top level item
		else
		{
			// update the pointers to the changed item
			m_changed_item = dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_changed_row));

			// set the elements from the stored values
			m_changed_item->setElements(m_new_command, m_new_value,
				m_new_show_msg, m_new_msg);
		}

		is_undo = 0;
	}
	else
	{
		// save new params
		m_last_command = m_changed_item->getLastCommand();
		m_last_value = m_changed_item->getLastValue();
		m_last_show_msg = m_changed_item->getLastSM();
		m_last_msg = m_changed_item->getLastMsg();

		m_new_command = m_changed_item->text(editorParams::c_command).toInt();
		m_new_value = m_changed_item->text(editorParams::c_value).toInt();
		m_new_show_msg = m_changed_item->checkState(editorParams::c_msg);
		m_new_msg = m_changed_item->text(editorParams::c_msg);

		// update pointers
		if (m_has_parent)
		{
			// get the parent
			m_parent = dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_parent_row));
			// get the child
			m_changed_item = dynamic_cast<protocolTreeWidgetItem *> (
				m_parent->child(m_changed_row));
		}
		else
		{
			// get the top level item
			m_changed_item = dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_changed_row));
		}

		if (m_has_parent)
		{
			this->setText("Changed child row " + QString::number(m_changed_row)
				+ " parent row " + QString::number(
									m_tree_widget->indexOfTopLevelItem(
										m_parent))
				+ " column " + QString::number(m_changed_column)
				+ " from " + QString::number(m_last_value)
				+ " to " + QString(m_changed_item->text(m_changed_column)));

		}
		else
		{
			this->setText("Changed top level item row " + QString::number(m_changed_row)
				+ " column " + QString::number(m_changed_column)
				+ " from " + QString::number(m_last_value)
				+ " to " + QString::number(m_changed_item->text(3).toInt()));
		}
	}
}

void changedProtocolCommand::undo()
{
	//	std::cout << HERE << std::endl;

	// if has parent
	if (m_has_parent)
	{
		// get the parent
		m_parent = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_parent_row));
		// get the child
		m_changed_item = dynamic_cast<protocolTreeWidgetItem *> (
			m_parent->child(m_changed_row));

		m_changed_item->setElements(m_last_command, m_last_value,
			m_last_show_msg, m_last_msg);
	}
	else
	{
		// get the top level item
		m_changed_item = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_changed_row));

		m_changed_item->setElements(m_last_command, m_last_value,
			m_last_show_msg, m_last_msg);
	}
	is_undo = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

moveUpCommand::moveUpCommand(
	QTreeWidget * const _tree_widget,
	protocolTreeWidgetItem * _parent):
	m_tree_widget(_tree_widget)
{

	m_parent = NULL;
	m_has_parent = false;
	m_row = m_tree_widget->currentIndex().row();

	if (_parent)
	{
		m_parent = _parent;
		m_has_parent = true;
		m_parent_row = _tree_widget->indexOfTopLevelItem(_parent);
	}
}

void moveUpCommand::redo() 
{

	if (m_has_parent) {

		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_parent->child(m_row));

		// take the child at the row
		m_parent->takeChild(m_row); 
		// add the selected item one row before
		m_parent->insertChild(m_row - 1, m_move_item);
	}
	else {
		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_row));

		// if we are on the top level, just take the item 
		m_tree_widget->takeTopLevelItem(m_row);
		// and add the selected item one row before
		m_tree_widget->insertTopLevelItem(m_row - 1, m_move_item);

		this->setText("Moved top level item at row " + QString::number(m_row) + 
			" to row " + QString::number(m_row - 1) );
	}
}

void moveUpCommand::undo()
{
	if (m_has_parent) {

		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_parent->child(m_row));

		m_parent->takeChild(m_row - 1); // take the child at the row
		m_parent->insertChild(m_row , m_move_item); // add the selected item one row before
	}
	else {
		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_row - 1));

		// if we are on the top level, just take the item 
		m_tree_widget->takeTopLevelItem(m_row - 1 );
		// and add the selected item one row before
		m_tree_widget->insertTopLevelItem(m_row, m_move_item);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

moveDownCommand::moveDownCommand(
	QTreeWidget * const _tree_widget,
	protocolTreeWidgetItem * _parent) :
	m_tree_widget(_tree_widget)
{

	m_parent = NULL;
	m_has_parent = false;
	m_row = m_tree_widget->currentIndex().row();
	// continue from here

	if (_parent)
	{
		m_parent = _parent;
		m_has_parent = true;
		m_parent_row = _tree_widget->indexOfTopLevelItem(_parent);
	}
}

void moveDownCommand::redo()
{

	if (m_parent) {

		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_parent->child(m_row));

		// take the child at the row
		m_parent->takeChild(m_row); 
		// add the selected item one row before
		m_parent->insertChild(m_row + 1, m_move_item); 
	}
	else {
		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_row));

		// if we are on the top level, just take the item 
		m_tree_widget->takeTopLevelItem(m_row);
		// and add the selected item one row before
		m_tree_widget->insertTopLevelItem(m_row + 1, m_move_item);

		this->setText("Moved top level item at row " + QString::number(m_row) +
			" to row " + QString::number(m_row + 1));
	}
}

void moveDownCommand::undo()
{
	if (m_parent) {

		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_parent->child(m_row));

		// take the child at the row
		m_parent->takeChild(m_row + 1); 
		// add the selected item one row before
		m_parent->insertChild(m_row, m_move_item); 
	}
	else {
		m_move_item =
			dynamic_cast<protocolTreeWidgetItem *> (
				m_tree_widget->topLevelItem(m_row + 1));

		// if we are on the top level, just take the item 
		m_tree_widget->takeTopLevelItem(m_row + 1);
		// and add the selected item one row before
		m_tree_widget->insertTopLevelItem(m_row, m_move_item);

	}
}