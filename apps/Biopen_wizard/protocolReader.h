/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_protocolReader_H_
#define Labonatip_protocolReader_H_

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

class Labonatip_protocolReader : public  QMainWindow
{
	Q_OBJECT

public:

	explicit Labonatip_protocolReader(QTreeWidget *_tree, QMainWindow *parent = nullptr);
	
	void switchLanguage(QString _translation_file);

	bool readProtocol(QString _filename);

private:

	bool decodeProtocolCommand(
		QByteArray &_command, protocolTreeWidgetItem &_out_item);
	
	void initCustomStrings();

	int getLevel(QTreeWidgetItem _item);

	std::vector<fluicell::PPC1api::command> *m_protocol;  //!< protocol to run

	QTreeWidget *m_tree;
	QString m_current_protocol_file_name;

	int m_cmd_idx_c;       // index of the column for command index
	int m_cmd_command_c;   // index of the column for the command
	int m_cmd_range_c;     // index of the column for the range
	int m_cmd_value_c;     // index of the column for the value
	int m_cmd_msg_c;       // index of the column for the command status message
	int m_cmd_level_c;     // index of the column for the level in the tree

	//custom translatable strings
	QString m_str_warning;
	QString m_str_file_not_found;
	QString m_str_negative_level;

	QTranslator m_translator_reader;

};

#endif /* Labonatip_protocolReader_H_ */