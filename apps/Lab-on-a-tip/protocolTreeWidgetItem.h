/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef PROTOCOL_TREE_WIDGET_ITEM_H_
#define PROTOCOL_TREE_WIDGET_ITEM_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_protocolEditor.h"
#include "protocolWizard.h"

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QTreeWidget>
#include <QObject>
#include <QItemDelegate>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>

#include "protocolEditorDelegates.h"
#include "dataStructures.h"

using namespace std;


class protocolTreeWidgetItem : public QWidget, public QTreeWidgetItem
{
	Q_OBJECT

public:
	protocolTreeWidgetItem(protocolTreeWidgetItem *parent = 0);

	bool protocolTreeWidgetItem::checkValidity( int _column);

	QString getRangeColumn(int _idx);

	void setElements(int _cmd_ind, int _value,
		bool _show_msg, QString _msg);

	protocolTreeWidgetItem * clone();

private:
	const pr_params * m_pr_params;

	const int m_cmd_idx_c;       // index of the column for command index
	const int m_cmd_command_c;   // index of the column for the command
	const int m_cmd_range_c;     // index of the column for the range
	const int m_cmd_value_c;     // index of the column for the value
	const int m_cmd_msg_c;       // index of the column for the command status message
	const int m_cmd_level_c;     // index of the column for the level in the tree
};


#endif /* PROTOCOL_TREE_WIDGET_ITEM_H_ */
