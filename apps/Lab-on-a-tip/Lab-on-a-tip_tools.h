/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_TOOLS_H_
#define Labonatip_TOOLS_H_

// standard libraries
#include <iostream>
#include <string>

#include "Lab-on-a-tip.h"
#include "ui_Lab-on-a-tip.h"

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>


using namespace std;

class Labonatip_macroRunner;

class Labonatip_tools : public  QDialog
{
	Q_OBJECT

signals :
	void ok();
	void apply();
	void discard();


public:

	// TODO: exclude QTserial and bring this structure to serial ! 
	// structure to handle COM port parameters
	struct COMSettings {
		std::string name;                    //--> COM1 --- n 
		int baudRate;                 //--> 4200 - 9600 --- 
		serial::bytesize_t dataBits;  //--> 5 - 6 - 7 - 8
		serial::parity_t parity;      //--> None, Even, Odd, Mark, Space 
		serial::stopbits_t stopBits;  //--> 1, 1.5, 2
		serial::flowcontrol_t flowControl;  //--> None, RTS/CTS, XON/XOFF 
		bool localEchoEnabled;

		COMSettings() {   // default values
			this->name = "COM5";
			this->baudRate = 115200;
			this->dataBits = serial::eightbits;
			this->parity = serial::parity_none;
			this->stopBits = serial::stopbits_one;
			this->flowControl = serial::flowcontrol_none;
		}

	}; // END COMSettings struct

  // structure to handle solutions name parameters
  struct solutionsNames {
	  QString sol1;                    //--> name of the solution 1
	  QString sol2;                    //--> name of the solution 2
	  QString sol3;                    //--> name of the solution 3
	  QString sol4;                    //--> name of the solution 4

	  solutionsNames() {   // default values
		  this->sol1 = "No name given to sol 1";
		  this->sol2 = "No name given to sol 2";
		  this->sol3 = "No name given to sol 3";
		  this->sol4 = "No name given to sol 4";
	  }
  }; // END solutionsNames struct

	 // structure to handle solutions name parameters
  struct pr_params {
	  int p_on_max;                    //!< name of the solution 1
	  int p_on_min;                     
	  int p_on_default;                     
	  int p_off_max;                     
	  int p_off_min;                     
	  int p_off_default;                    
	  int v_switch_max;                     
	  int v_switch_min;                  
	  int v_switch_default;               
	  int v_recirc_max;
	  int v_recirc_min;
	  int v_recirc_default;

	  pr_params() : p_on_max (0), p_on_min(0), p_on_default(0),
				    p_off_max(0), p_off_min(0), p_off_default(0),
				    v_switch_max(0), v_switch_min(0), v_switch_default(0),
					v_recirc_max(0), v_recirc_min(0), v_recirc_default(0)
	  {   // default values
	  }
  }; // END pr_params struct

	explicit Labonatip_tools(QWidget *parent = 0);

	~Labonatip_tools();


	// TODO: members should not be public .... add get functions instead
	COMSettings *m_comSettings;
	solutionsNames *m_solutionNames;
	pr_params *m_pr_params;

	void setMacroPrt(std::vector<fluicell::PPC1api::command> *_macro) { m_macro = _macro; };



private slots:


/** Visualize the about dialog
* \note
*/
	void onListMailItemClicked(QListWidgetItem* _item);

	void showPortInfo(int idx);

	void okPressed();

	void discardPressed();

	void applyPressed();

	void enumerate();

	bool loadMacro( );

	bool saveMacro( );

	void clearAllCommands();

	void addMacroCommand();
	
	void removeMacroCommand();

	void becomeChild();

	void becomeParent();
	
	void moveUp();

	void moveDown();

	void plusIndent();

	bool checkValidity(QTreeWidgetItem *_item, int _column);

	void duplicateItem();

	void visualizeItemProperties();

	/** Load an ini setting file
	* in the GUI initialization it takes a default value ./settings/setting.ini
	* \note
	*/
	void loadSettings(QString _path);

	/** Load an ini setting file overload to the default path
	* \note
	*/
	void loadSettings() { loadSettings(QString("")); }

	/** save the setting file
	*  
	* \note
	*/
	void saveSettings();


	void addAllCommandsToMacro();


private:

	QSettings *m_settings;

	void createNewCommand(QTreeWidgetItem &_command);

	QString createHeader();
	
	bool loadMacro(const QString _file_name);
	
	bool saveMacro(QString _file_name);
	
	void getLastNode(QTreeWidget *_tree, QTreeWidgetItem *_item);

	int getLevel(QTreeWidgetItem _item);

	void visitTree(QTreeWidgetItem *_item);

	void visitTree(QList<QStringList> &_list, QTreeWidgetItem *_item);

	QList<QStringList> visitTree(QTreeWidget *_tree);

	bool decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item);

	void getCOMsettings();

	void getSolutionSettings();

	std::vector<fluicell::PPC1api::command> *m_macro;

protected:
	Ui::Labonatip_tools *ui_tools;    //--> the user interface
};


#endif /* Labonatip_TOOLS_H_ */
