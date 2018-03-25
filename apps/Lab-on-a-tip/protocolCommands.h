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


class addProtocolCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	addProtocolCommand(
		QTreeWidget * const _tree_widget,
		int _at_row,
		protocolTreeWidgetItem * _parent = 0);

	protocolTreeWidgetItem * item() { return m_new_item; };

	void redo();
	void undo();

private:

	QTreeWidget * const m_tree_widget;
	protocolTreeWidgetItem * m_new_item;
	protocolTreeWidgetItem * m_parent;
	

	int const m_at_row;
	int is_undo;

	int m_view_idx;
	int m_cmd_idx;
	int m_value;
	bool m_show_status_msg;
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

	int const m_at_row;

	int m_view_idx;
	int m_cmd_idx;
	int m_value;
	bool m_show_status_msg;
	QString m_status_msg;

};


class changedProtocolCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	changedProtocolCommand(
		QTreeWidget * const _tree_widget,
		protocolTreeWidgetItem * _changed_item,
		int _row, int _column,
		protocolTreeWidgetItem * _parent = 0);

	void redo();
	void undo();

private:

	QTreeWidget const *  m_tree_widget;
	protocolTreeWidgetItem * m_changed_item;
	protocolTreeWidgetItem * m_parent;
	int const m_changed_row;
	int const m_changed_column;
	
	int m_view_idx;
	int m_cmd_idx;
	int m_value;
	bool m_show_status_msg;
	QString m_status_msg;

};

class protocolStackCommand : public QTreeWidget, public QUndoCommand
{
	Q_OBJECT
public:

	protocolStackCommand(
		QTreeWidget * const _tree_widget,
		QList<QStringList> _protocol_last,
		QList<QStringList> _protocol_new);

	void redo();
	void undo();

private:

	QTreeWidget const *  m_tree_widget;
	QList<QStringList> m_protocol_last;
	QList<QStringList> m_protocol_new;
	

};


#endif /* PROTOCOL_COMMANDS_H_ */