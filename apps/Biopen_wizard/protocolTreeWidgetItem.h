/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef PROTOCOL_TREE_WIDGET_ITEM_H_
#define PROTOCOL_TREE_WIDGET_ITEM_H_

// standard libraries
#include <iostream>
#include <string>

//#include "ui_protocolEditor.h"
//#include "protocolWizard.h"

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


/* Protocol widget item, this inherit the QTreeWidget methods
   
*/
class protocolTreeWidgetItem : public QWidget, public QTreeWidgetItem
{
	Q_OBJECT

public:
	// ctor
	protocolTreeWidgetItem(protocolTreeWidgetItem *parent = 0);

	// check validity and do some reset 
    bool checkValidity( int _column);

	// the range column is static,
	// this allow to get the static value to put in
	QString getRangeColumn(int _idx);

	// allows to re-set the element with all the columns
	void setElements(int _cmd_ind, int _value,
		bool _show_msg, QString _msg);

	// virtual re-implementation of set data
	void setData(int column, int role, const QVariant &value);

	// virtual in QTreeWidgetItem, to re-implement 
	protocolTreeWidgetItem * clone();

	int getLastCommand() { return m_last_command; }
	int getLastValue() { return m_last_value; }
	Qt::CheckState getLastSM() { return m_last_show_msg; }
    QString getLastMsg() { return QString(m_last_show_msg); }

private:
	
	// just to keep track of the last values changed
	int m_last_command;
	int m_last_value;
	Qt::CheckState m_last_show_msg;
	QString m_last_msg;

	// params for the settings of the PPC1
	const pr_params * m_pr_params; 

	// index of the column for command index							   
	const int m_cmd_idx_c;         
	
	// index of the column for the command
	const int m_cmd_command_c;

	// index of the column for the range
	const int m_cmd_range_c;       

	// index of the column for the value
	const int m_cmd_value_c;      
	
	// index of the column for the command status message
	const int m_cmd_msg_c;         
	
	// index of the column for the level in the tree
	const int m_cmd_level_c;       
};


#endif /* PROTOCOL_TREE_WIDGET_ITEM_H_ */
