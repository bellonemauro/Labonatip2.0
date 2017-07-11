/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
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

/** Create signals to be passed to the main app
* 
*/
signals :
	void ok();  // generated when ok is pressed
	void apply(); // generated when apply is pressed
	void discard(); // generated when discard is pressed


public:

	// TODO: exclude QTserial and bring this structure to serial ! 
	// structure to handle COM port parameters
	struct COMSettings {
		std::string name;                    //!<  COM1 --- n 
		int baudRate;                        //!<  4200 - 9600 --- 
		serial::bytesize_t dataBits;         //!<  5 - 6 - 7 - 8
		serial::parity_t parity;             //!<  None, Even, Odd, Mark, Space 
		serial::stopbits_t stopBits;         //!<  1, 1.5, 2
		serial::flowcontrol_t flowControl;   //!<  None, RTS/CTS, XON/XOFF 
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
	  QString sol1;                    //!<  name of the solution 1
	  QString sol2;                    //!<  name of the solution 2
	  QString sol3;                    //!<  name of the solution 3
	  QString sol4;                    //!<  name of the solution 4

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

	  pr_params() : p_on_max (450), p_on_min(0), p_on_default(190),
				    p_off_max(450), p_off_min(0), p_off_default(21),
				    v_switch_max(0), v_switch_min(-300), v_switch_default(-115),
					v_recirc_max(0), v_recirc_min(-300), v_recirc_default(-115)
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

	/** Allows to switch windows tabs in the interface from the list widget
	* \note
	*/
	void onListMailItemClicked(QListWidgetItem* _item);

	/** Show information for the serial port communication
	* \note
	*/
	void showPortInfo(int idx);

	/** emit ok signal, save the setting, send the current macro to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit discard signal and close the window
	* \note
	*/
	void discardPressed();

	/** emit apply signal, save the setting, and send the current macro to the main
	*   
	*/
	void applyPressed();

	/** Enumerate serial ports
	*
	*/
	void enumerate();

	/** Load a macro fron file, only one type of macro is currently supported
	*
	*/
	bool loadMacro( );

	/** Save the macro to file
	*
	*/
	bool saveMacro( );

	/** Clear all the macro commands
	*
	*/
	void clearAllCommands();

	/** Add a new macro command
	*
	*/
	void addMacroCommand();

	/** remove a macro command
	*
	*/
	void removeMacroCommand();

	/** The selected element will become a child for the preceding element
	*
	*/
	void becomeChild();

	/** The selected element will become parent 
	*
	*/
	void becomeParent();
	
	void moveUp();

	void moveDown();

	void plusIndent();

	void duplicateItem();

//	void visualizeItemProperties(); // deprecated

	bool checkValidity(QTreeWidgetItem *_item, int _column);

	void commandChanged(int _idx);

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

	/** Put all the commands in the macro editor to the macro structure for running
	*
	* \note
	*/
	void addAllCommandsToMacro();


private:

	QSettings *m_settings;

	//void createNewCommand(QTreeWidgetItem &_command); // deprecated

	void createNewCommand(QTreeWidgetItem &_command, QComboBox &_combo_box);
	
	/** overload to allow creating the combobox only without the item
	*
	*/
	void createNewCommand(QComboBox &_combo_box) {
		QTreeWidgetItem item;
		createNewCommand(item, _combo_box);
	}
		
	QString createHeader();
	
	bool loadMacro(const QString _file_name);
	
	bool saveMacro(QString _file_name);
	
	void getLastNode(QTreeWidget *_tree, QTreeWidgetItem *_item);

	int getLevel(QTreeWidgetItem _item);

	//void visitTree(QTreeWidgetItem *_item);

	void visitTree(QList<QStringList> &_list, QTreeWidget *_tree, QTreeWidgetItem *_item);

	QList<QStringList> visitTree(QTreeWidget *_tree);

	bool decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item);

	void getCOMsettings();

	void getSolutionSettings();

	std::vector<fluicell::PPC1api::command> *m_macro;


protected:
	Ui::Labonatip_tools *ui_tools;    //!<  the user interface
};


#endif /* Labonatip_TOOLS_H_ */
