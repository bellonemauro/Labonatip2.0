/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef PROTOCOL_COMMANDS_H_
#define PROTOCOL_COMMANDS_H_

// standard libraries
#include <iostream>
#include <string>

// Qt libraries
#include <QUndoCommand>
#include <QTreeWidget>
#include <QObject>

#include "protocolTreeWidgetItem.h"


// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>



/**  Add protocol command to create a new command in the 
  *  protocol tree widget and push it into the undo stack,
  *  the constructor also calls the first time
  *  the function redo
  *   
  **/
class addProtocolCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	// ctor
	addProtocolCommand(
		QTreeWidget * const _tree_widget,
		int _at_row,
		protocolTreeWidgetItem * _parent = 0);

	// just return the widget upon request
	protocolTreeWidgetItem * item() { return m_new_item; };

	// reimplementation of the virtual method  
	void redo();

	// reimplementation of the virtual method  
	void undo();

private:

	// pointer to the tree widget
	QTreeWidget * const m_tree_widget;   

	// the new item to be added
	protocolTreeWidgetItem * m_new_item;

	// parent to the new item (if any)
	protocolTreeWidgetItem * m_parent;
	bool m_has_parent;
	int m_parent_row;

	// row to add the command
	int const m_at_row;

	// track the undo/redo
	int is_undo;

	// data members of the line that is added / removed, 
	// I am using this to undo/redo the command as 
	// the pointer gets destroyed
	int m_view_idx;
	int m_cmd_idx;
	int m_value;
	Qt::CheckState m_show_status_msg;
	QString m_status_msg;

};




class removeProtocolCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	removeProtocolCommand(
		QTreeWidget * const _tree_widget,
		int _at_row,
		protocolTreeWidgetItem * _parent = 0);

	void redo();
	void undo();

private:

	QTreeWidget * const m_tree_widget;
	protocolTreeWidgetItem * m_remove_item;
	
	protocolTreeWidgetItem * m_parent;
	bool m_has_parent;
	int m_parent_row;

	int const m_at_row;

	int m_view_idx;
	int m_cmd_idx;
	int m_value;
	Qt::CheckState m_show_status_msg;
	QString m_status_msg;

};


class changedProtocolCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	changedProtocolCommand(
		QTreeWidget * const _tree_widget,
		protocolTreeWidgetItem * _changed_item,
		int _column,
		protocolTreeWidgetItem * _parent = 0);

	void redo();
	void undo();

private:

	QTreeWidget const *  m_tree_widget;
	protocolTreeWidgetItem * m_changed_item;
	protocolTreeWidgetItem * m_parent;
	bool m_has_parent;
	int m_parent_row;

	int m_changed_row;
	int m_changed_column;
	
	// just to keep track of the last values changed
	int m_last_command;
	int m_last_value;
	Qt::CheckState m_last_show_msg;
	QString m_last_msg;

	int m_new_command;
	int m_new_value;
	Qt::CheckState m_new_show_msg;
	QString m_new_msg;

	bool is_undo;

};


class moveUpCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	moveUpCommand(
		QTreeWidget * const _tree_widget,
		int _row, 
		protocolTreeWidgetItem * _parent = 0);

	void redo();
	void undo();

private:

	QTreeWidget const *  m_tree_widget;
	protocolTreeWidgetItem * m_move_item;
	protocolTreeWidgetItem * m_parent;
	int const m_row;


	// just to keep track of the last values changed
	int m_command;
	int m_value;
	Qt::CheckState m_show_msg;
	QString m_msg;

	int m_new_command;
	int m_new_value;
	Qt::CheckState m_new_show_msg;
	QString m_new_msg;

	bool is_undo;

};


class moveDownCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	moveDownCommand(
		QTreeWidget * const _tree_widget,
		int _row,
		protocolTreeWidgetItem * _parent = 0);

	void redo();
	void undo();

private:

	QTreeWidget const *  m_tree_widget;
	protocolTreeWidgetItem * m_move_item;
	protocolTreeWidgetItem * m_parent;
	int const m_row;


	// just to keep track of the last values changed
	int m_command;
	int m_value;
	Qt::CheckState m_show_msg;
	QString m_msg;

	int m_new_command;
	int m_new_value;
	Qt::CheckState m_new_show_msg;
	QString m_new_msg;

	bool is_undo;

};

#endif /* PROTOCOL_COMMANDS_H_ */