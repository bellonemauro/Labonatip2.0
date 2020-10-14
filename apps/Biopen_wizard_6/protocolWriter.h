/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
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
#include <fluicell/ppc1api6/ppc1api6.h>

/** \brief New class for the protocol writer
*
*   In this class we have the protocol writer with the interpreter for
*   the data files, but also the dialog windows
*   This will be probably deprecated ones the new xml format will be used instead
*
*/
class protocolWriter : public  QMainWindow
{
	Q_OBJECT

public:

	/** \brief Constructor, 
	*
	*/
	protocolWriter(QMainWindow *parent = nullptr) {};

	/** \brief Switch the language in the GUI
	*
	*  @param _translation_file contains the language file to be loaded
	*/
	void switchLanguage(QString _translation_file);

	/** \brief Write a protocol to file
	*
	*   The protocol widget in the GUI will be read and saved to a file
	*
	*   \return false for any error, e.g. file not found, protocol corrupted
	*
	*  @param _filename complete path to the file to be saved
	*/
	bool saveProtocol(const QTreeWidget *_tree, QString _file_name);

private:

	/** \brief Initialize the custom strings to allow translations
	*
	*/
	void initCustomStrings();	
	
	/** \brief Visit the tree and pull all the commands into the _list
	*
	*    Here the top level items are visited and call the overload function
	*    for each element (that can also be a sublevel of the tree)
	*
	*/
	QList<QStringList> visitTree(const QTreeWidget *_tree);

	/** \brief Visit the tree and pull all the commands into the _list
	*
	*/
	void visitTree(QList<QStringList> &_list,
		const QTreeWidget *_tree, QTreeWidgetItem *_item);


	/** \brief Create the header
	*
	*/
	QString createHeader();

	//custom translatable strings
	QString m_str_warning;
	QString m_str_information;
	QString m_str_check_validity_protocol;
	QString m_str_check_validity_protocol_try_again;
	QString m_str_file_not_saved;
	QString m_ask_password;
	QString m_wrong_password;
	QString m_correct_password;

	QTranslator m_translator_writer;
};

#endif /* Labonatip_protocolReader_H_ */