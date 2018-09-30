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
#include <QMessageBox>
#include <QDateTime>

#include "protocolTreeWidgetItem.h"
#include "protocolCommands.h"

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>

/** \brief New class for the protocol reader
*
*   In this class we have the protocol reader with the interpreter for 
*   the data files, but also the dialog windows
*   This will be probably deprecated ones the new xml format will be used instead
*
*/
class protocolReader : public  QMainWindow
{
	Q_OBJECT

public:

	/** \brief Constructor, initialize objects and parameters using default values
	*
	*/
	explicit protocolReader(QMainWindow *parent = nullptr) {};
	
	/** \brief Switch the language in the GUI
	*
	*  @param _translation_file contains the language file to be loaded
	*/
	void switchLanguage(QString _translation_file);

	/** \brief Read protocol in _filename
	*
	*   The file will be read entirely and then decoded line by line using decodeProtocolCommand(.).
	*   
	*
	*   \return false for any error, e.g. file not found, protocol file corrupted
	*
	*  @param _tree the protocol will be loaded into this tree widget
	*  @param _filename complete path to the file to be loaded
	*/
	bool readProtocol(QTreeWidget *_tree, QString _filename);

private:

	/** \brief Initialize the custom strings to allow translations
	*
	*/
	void initCustomStrings();

	/** \brief Decode each line of the protocol file
	*
	*   Here we expect a line like this: 13#1#message#0#
	*
	*   Where the values indicate: Command#Value#status_message#depth
	*
	*   \return false for any error, e.g. file not found, protocol file corrupted
	*
	*  @param _command is the input command line as a string
	*  @param _out_item is the output widget item
	*/
	bool decodeProtocolCommand(
		QByteArray &_command, protocolTreeWidgetItem &_out_item);
	

	/** \brief Return the level fo the data in the widget to allow loops in the protocol file
	*
	*/
	int getLevel(QTreeWidgetItem _item);


	/** \brief Check the protocol version for backward compatibility
	*
	*   Return the protocol version from a line of the protocol, it expect a line 
	*   which must have the following structure "%% Protocol Header V. 0.x \n" 
	*   this is written in the protocol file header by default.
	*   The version must be higher than zero, otherwise something is wrong
	*
	*   \return 0 if the protocol version is not readable, e.g. wrong line 
	*
	*/
	int checkProtocolVersion(QByteArray _command);

	/** \brief Command remapping for backward compatibility
	*
	*   ATTENTION: this function may be deprecated soon. 
	*
	*   During the development we have changed the protocol files many times
	*   hence this is a way to keep track of the important versions and support compatibility
	*
	*   The function checks the _version and return the command index converted according to 
	*   the following map:
	*
	*  First mapping    Second mapping    Third mapping
	*
	*  setPon = 0,      zoneSize = 0,     setZoneSize = 0,
	*  setPoff = 1,     flowSpeed = 1,    changeZoneSizeBy = 1,
	*  setVswitch = 2,  vacuum = 2,       setFlowSpeed = 2,
	*  setVrecirc = 3,  wait = 3,         changeFlowSpeedBy = 3,
	*  solution1 = 4,   allOff = 4,       setVacuum = 4,
	*  solution2 = 5,   solution1 = 5,    changeVacuumBy = 5,
	*  solution3 = 6,   solution2 = 6,    wait = 6,
	*  solution4 = 7,   solution3 = 7,    allOff = 7,
	*  wait = 8,        solution4 = 8,    solution1 = 8,
	*  ask_msg = 9,     setPon = 9,       solution2 = 9,
	*  allOff = 10,     setPoff = 10,     solution3 = 10,
	*  pumpsOff = 11,   setVrecirc = 11,  solution4 = 11,
	*  waitSync = 12,   setVswitch = 12,  setPon = 12,
	*  syncOut = 13,	   ask_msg = 13,     setPoff = 13,
	*  zoneSize = 14,   pumpsOff = 14,    setVrecirc = 14,
	*  flowSpeed = 15,  waitSync = 15,    setVswitch = 15,
	*  vacuum = 16,     syncOut = 16,     ask_msg = 16,
	*  loop = 17        loop = 17         pumpsOff = 17,
	*									 waitSync = 18,
	*								 	 syncOut = 19,
	*									 loop = 20,
	*/
	QString remapForBackwardCompatibility( int _version, QString _old_data);

	// data members:

	std::vector<fluicell::PPC1api::command> *m_protocol;  //!< protocol to run



	//custom translatable strings
	QString m_str_warning;
	QString m_str_file_not_found;
	QString m_str_negative_level;

	QTranslator m_translator_reader;

};

#endif /* Labonatip_protocolReader_H_ */