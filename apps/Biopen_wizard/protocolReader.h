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

	int checkProtocolVersion(QByteArray _command);

	QString remapForBackwardCompatibility( int _version, QString _old_data);

	std::vector<fluicell::PPC1api::command> *m_protocol;  //!< protocol to run

	QTreeWidget *m_tree;
	QString m_current_protocol_file_name;

	editorParams *m_editor_params;

	int m_protocol_version;

	//custom translatable strings
	QString m_str_warning;
	QString m_str_file_not_found;
	QString m_str_negative_level;

	QTranslator m_translator_reader;

};

#endif /* Labonatip_protocolReader_H_ */