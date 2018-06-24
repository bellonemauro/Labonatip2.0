/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_protocolWriter_H_
#define Labonatip_protocolWriter_H_

// standard libraries
#include <iostream>
#include <string>

// Qt
#include <QTreeWidget>
#include <QDialog>
#include <QTranslator>
#include <QApplication>

#include "protocolTreeWidgetItem.h"
#include "protocolCommands.h"

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>

class Labonatip_protocolWriter : public  QMainWindow
{
	Q_OBJECT

public:

	explicit Labonatip_protocolWriter(QTreeWidget *_tree, QMainWindow *parent = nullptr);

	void switchLanguage(QString _translation_file);


	bool saveProtocol(QString _file_name);


private:

	void visitTree(QList<QStringList> &_list,
		QTreeWidget *_tree, QTreeWidgetItem *_item);
	QList<QStringList> visitTree(QTreeWidget *_tree);

	QString createHeader();
	void initCustomStrings();



	std::vector<fluicell::PPC1api::command> *m_protocol;  //!< protocol to run

	QTreeWidget *m_tree;

	int m_cmd_idx_c;       // index of the column for command index
	int m_cmd_command_c;   // index of the column for the command
	int m_cmd_range_c;     // index of the column for the range
	int m_cmd_value_c;     // index of the column for the value
	int m_cmd_msg_c;       // index of the column for the command status message
	int m_cmd_level_c;     // index of the column for the level in the tree

	//custom translatable strings
	QString m_str_warning;
	QString m_str_check_validity_protocol;
	QString m_str_check_validity_protocol_try_again;
	QString m_str_file_not_saved;

	QTranslator m_translator_writer;
};

#endif /* Labonatip_protocolReader_H_ */