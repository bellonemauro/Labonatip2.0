/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolCommands.h"

#include <QTime>


addProtocolCommand::addProtocolCommand(
	QTreeWidget * const _tree_widget, int _at_row, 
	protocolTreeWidgetItem * _parent)
	: m_tree_widget(_tree_widget),
	m_at_row(_at_row)
{
	
	is_undo = 0;
	m_parent = NULL;

	// if we have a parent we save it into a member
	if (_parent)
	{
		m_parent = _parent;
	}

}

void addProtocolCommand::redo()
{
	//std::cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "addProtocolCommand::redo " << std::endl;
	
	// create the new command to add
	m_new_item = new protocolTreeWidgetItem();

	// if this is the redo runned after and undo
	// the object needs to be re-created as the 
	// undo delete the pointer, so now the object
	// m_new_item is pointing to another location
	if (is_undo == 1)
	{
		// if has a parent, add as a child
		if (m_parent)
		{// re-create the item as it was 
			m_new_item->setElements(
				m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
			m_parent->insertChild(m_at_row, m_new_item);
		}
		else
		{// re-create the item as it was 
			m_new_item->setElements(
				m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
			m_tree_widget->insertTopLevelItem(m_at_row, m_new_item);
		}
	}

	// if we have a parent, the new item is added as a child
	if (m_parent)
	{
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
	//std::cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "addProtocolCommand::undo " << std::endl;

	//TODO: it crashes when undo an already undid-deleted command
	//     add -> remove -> undo(remove) -> undo (add)=>crash!
	//     the reason is that the remove_command destroy the pointer 
	//     and the undo_remove creates a new pointer somewhere else
	//     so to undo (add) we need to know the new pointers of the 
	//     undo-removed object
	if (m_tree_widget->topLevelItem(m_at_row) == NULL)
	{
		int i = 0;
	}
	//TODO: this is just a trick and it will not work in the long run
	//m_new_item = dynamic_cast<protocolTreeWidgetItem *> (
	//	m_tree_widget->topLevelItem(m_at_row));

	if ( m_new_item->QTreeWidgetItem::parent())
	{
		m_parent = 
			dynamic_cast<protocolTreeWidgetItem *> (
				m_new_item->QTreeWidgetItem::parent());

		// get the item to be removed
		m_new_item = dynamic_cast<protocolTreeWidgetItem *> (
			m_parent->child(m_at_row));

		//save the params 
		m_view_idx = m_new_item->text(0).toInt();
		m_cmd_idx = m_new_item->text(1).toInt();
		m_value = m_new_item->text(3).toInt();
		m_show_status_msg = m_new_item->checkState(4);
		m_status_msg = m_new_item->text(4);

		// remove the item
		m_parent->takeChild(m_at_row);
	}
	else {

		// get the item to be removed
		m_new_item = dynamic_cast<protocolTreeWidgetItem *> (
			m_tree_widget->topLevelItem(m_at_row));

		//save the params 
		m_view_idx = m_new_item->text(0).toInt();
		m_cmd_idx = m_new_item->text(1).toInt();
		m_value = m_new_item->text(3).toInt();
		m_show_status_msg = m_new_item->checkState(4);
		m_status_msg = m_new_item->text(4);

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

	m_parent = NULL;
	if (_parent)
	{
		m_parent = _parent;
	}
}



void removeProtocolCommand::redo()
{
	//std::cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "removeProtocolCommand::redo " << std::endl;

	
	if (m_parent)
	{
		m_remove_item = dynamic_cast<protocolTreeWidgetItem *>(
			m_parent->child(m_at_row));
		// save params
		m_view_idx = m_remove_item->text(0).toInt();
		m_cmd_idx = m_remove_item->text(1).toInt();
		m_value = m_remove_item->text(3).toInt();
		m_show_status_msg = m_remove_item->checkState(4);
		m_status_msg = m_remove_item->text(4);

		m_parent->takeChild(m_at_row);

		int rr = m_tree_widget->indexOfTopLevelItem(m_parent);
		this->setText("Removed child at " + QString::number(m_at_row)
			+ " parent row " + QString::number(rr));

	}
	else
	{
		m_remove_item = dynamic_cast<protocolTreeWidgetItem *>(
			m_tree_widget->topLevelItem(m_at_row)
			);

		// save params
		m_view_idx = m_remove_item->text(0).toInt();
		m_cmd_idx = m_remove_item->text(1).toInt();
		m_value = m_remove_item->text(3).toInt();
		m_show_status_msg = m_remove_item->checkState(4);
		m_status_msg = m_remove_item->text(4);

		m_tree_widget->model()->removeRow(m_at_row);
		this->setText("Removed top level item at " + QString::number(m_at_row));
	}	
}

void removeProtocolCommand::undo()
{
	//std::cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "removeProtocolCommand::undo " << std::endl;

	m_remove_item = new protocolTreeWidgetItem();
	if (m_parent)
	{// re-create the item as it was 
		m_remove_item->setElements(
			m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
		m_parent->insertChild(m_at_row, m_remove_item);
	}
	else
	{// re-create the item as it was 
		m_remove_item->setElements(
			m_cmd_idx, m_value, m_show_status_msg, m_status_msg);
		m_tree_widget->insertTopLevelItem(m_at_row, m_remove_item);
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////

changedProtocolCommand::changedProtocolCommand(
	QTreeWidget * const _tree_widget,
	protocolTreeWidgetItem * _changed_item,
	int _row, int _column,
	protocolTreeWidgetItem * _parent) :
	m_tree_widget(_tree_widget),
	m_changed_row(_row),
	m_changed_column (_column)
{
	is_undo = 0;
	m_parent = NULL;
	if (_parent)
	{
		m_parent = _parent;
	}

	m_changed_item = _changed_item;
}



void changedProtocolCommand::redo()
{
	//std::cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "changedProtocolCommand::redo " << std::endl;

	// save new params
	if (is_undo == 1)
	{
		//m_changed_item->setText(1, QString::number(m_new_command));
		//m_changed_item->setText(3, QString::number(m_new_value));
		//m_changed_item->setCheckState(4, Qt::CheckState(m_new_show_msg));
		//m_changed_item->setText(4, m_new_msg);
		m_changed_item->setElements(m_new_command, m_new_value, m_new_show_msg, m_new_msg);
		is_undo = 0;
	}
	else
	{
		m_last_command = m_changed_item->getLastCommand();
		m_last_value = m_changed_item->getLastValue();
		m_last_show_msg = m_changed_item->getLastSM();
		m_last_msg = m_changed_item->getLastMsg();

		m_new_command = m_changed_item->text(1).toInt();
		m_new_value = m_changed_item->text(3).toInt();
		m_new_show_msg = m_changed_item->checkState(4);
		m_new_msg = m_changed_item->text(4);

		int rr = m_tree_widget->currentIndex().row();

		if (m_tree_widget->currentItem()->parent())
		{
			this->setText("Changed child row " + QString::number(rr) //QString::number(m_changed_row)
				+ " parent row " + QString::number(
									m_tree_widget->indexOfTopLevelItem(
										m_tree_widget->currentItem()->parent()))
				+ " column " + QString::number(m_changed_column)
				+ " from " + QString::number(m_last_value)
				+ " to " + QString(m_changed_item->text(m_changed_column)));

		}
		else
		{
			this->setText("Changed top level item row " + QString::number(rr) //QString::number(m_changed_row)
				+ " column " + QString::number(m_changed_column)
				+ " from " + QString::number(m_last_value)
				+ " to " + QString::number(m_changed_item->text(3).toInt()));
		}

	}

}

void changedProtocolCommand::undo()
{
	//std::cout << QDate::currentDate().toString().toStdString() << "  "
	//	<< QTime::currentTime().toString().toStdString() << "  "
	//	<< "changedProtocolCommand::undo " << std::endl;
	//m_changed_item->setText(1, QString::number(m_last_command));
	//m_changed_item->setText(3, QString::number(m_last_value));
	//m_changed_item->setCheckState(4, Qt::CheckState(m_last_show_msg));
	//m_changed_item->setText(4, m_last_msg);
	m_changed_item->setElements(m_last_command, m_last_value, m_last_show_msg, m_last_msg);
	is_undo = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

