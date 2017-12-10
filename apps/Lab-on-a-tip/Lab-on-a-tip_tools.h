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

#include "ui_Lab-on-a-tip_tools.h"
#include <QMainWindow>
#include <QSettings>
#include <QTranslator>
#include <QDateTime>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>


using namespace std;



class Labonatip_tools : public  QMainWindow
{
	Q_OBJECT

/** Create signals to be passed to the main app
* 
*/
signals :
	void ok();  // generated when ok is pressed
	void apply(); // generated when apply is pressed
	void cancel(); // generated when cancel is pressed
	void emptyWells(); // generated when empty wells is pressed
	void colSol1Changed(const int _r, const int _g, const int _b);
	void colSol2Changed(const int _r, const int _g, const int _b);
	void colSol3Changed(const int _r, const int _g, const int _b);
	void colSol4Changed(const int _r, const int _g, const int _b);


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
  struct solutionsParams {
	  int volume_sol1;                 //!<  volume of solution in the well 1
	  int volume_sol2;                 //!<  volume of solution in the well 2
	  int volume_sol3;                 //!<  volume of solution in the well 3
	  int volume_sol4;                 //!<  volume of solution in the well 4
	  QString sol1;                    //!<  name of the solution 1
	  QString sol2;                    //!<  name of the solution 2
	  QString sol3;                    //!<  name of the solution 3
	  QString sol4;                    //!<  name of the solution 4
	  QColor sol1_color;               //!<  color of the solution 1
	  QColor sol2_color;               //!<  color of the solution 2
	  QColor sol3_color;               //!<  color of the solution 3
	  QColor sol4_color;               //!<  color of the solution 4

	  solutionsParams() {   // default values
		  this->volume_sol1 = 0;
		  this->volume_sol2 = 0;
		  this->volume_sol3 = 0;
		  this->volume_sol4 = 0;
		  this->sol1 = "No name given to sol 1";
		  this->sol2 = "No name given to sol 2";
		  this->sol3 = "No name given to sol 3";
		  this->sol4 = "No name given to sol 4";
		  this->sol1_color = QColor::fromRgb(255, 189, 0);
		  this->sol2_color = QColor::fromRgb(255, 40, 0);
		  this->sol3_color = QColor::fromRgb(0, 158, 255);
		  this->sol4_color = QColor::fromRgb(130, 255, 0);
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
	  int base_ds_increment;
	  int base_fs_increment;
	  int base_v_increment;

	  pr_params() : p_on_max (450), p_on_min(0), p_on_default(190),
				    p_off_max(450), p_off_min(0), p_off_default(21),
				    v_switch_max(0), v_switch_min(-300), v_switch_default(-115),
					v_recirc_max(0), v_recirc_min(-300), v_recirc_default(-115),
					base_ds_increment(10), base_fs_increment(5), base_v_increment(5)
	  {   // default values
	  }
  }; // END pr_params struct


  struct GUIparams {
	  Qt::ToolButtonStyle showTextToolBar;     //!<  ToolButtonIconOnly --- n 
	  bool enableToolTips;

	  GUIparams() {   // default values
		  this->showTextToolBar = Qt::ToolButtonIconOnly;
		  this->enableToolTips = false;
	  }

  }; // END COMSettings struct

	explicit Labonatip_tools(QWidget *parent = 0);

	~Labonatip_tools();

	// TODO: members should not be public .... add get functions instead
	COMSettings *m_comSettings;
	solutionsParams *m_solutionParams;
	pr_params *m_pr_params;
	GUIparams *m_GUI_params;

	int language; //TODO add an enumerator

	void switchLanguage(QString _translation_file);

	void updateDevices() { this->enumerate(); }

	bool setLoadSettingsFileName(QString _filename) { 
		m_setting_file_name = _filename; 
		return loadSettings(m_setting_file_name);
	}

	bool setSaveSettingsFileName(QString _filename) {
		m_setting_file_name = _filename;
		return saveSettings(_filename);
	}

	void setDefaultPressuresVacuums(int _p_on_default, int _p_off_default, 
		                            int _v_recirc_default, int _v_switch_default);


	double getSolutionTime() { return (double)ui_tools->doubleSpinBox_solution->value(); }

	bool isContinuousFlowing() { return ui_tools->checkBox_disableTimer->isChecked(); }

private slots:

	void goToPage1();
	void goToPage2();
	void goToPage3();
	void goToPage4();

	/** Show information for the serial port communication
	* \note
	*/
	void showPortInfo(int idx);

	void languageChanged(int _idx);

	void toolButtonStyleChanged(int _idx);

	void enableToolTip(int _inx);


	/** Color solution 1 changed
	* \note
	*/
	void colorSol1Changed(int _value);

	/** Color solution 2 changed
	* \note
	*/
	void colorSol2Changed(int _value);

	/** Color solution 3 changed
	* \note
	*/
	void colorSol3Changed(int _value);

	/** Color solution 4 changed
	* \note
	*/
	void colorSol4Changed(int _value);

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
	void setContinuousFow(int _state) {
		  ui_tools->doubleSpinBox_solution->setEnabled(!_state); // TODO !!! this is still not in the setting file
	}

	/** emit ok signal, save the setting, send the current macro to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit cancel signal and close the window
	* \note
	*/
	void cancelPressed();

	/** emit apply signal, save the setting, and send the current macro to the main
	*   
	*/
	void applyPressed();


	void emptyWellsPressed();

	/** Enumerate serial ports
	*
	*/
	void enumerate();


	/** Load an ini setting file
	* in the GUI initialization it takes a default value ./settings/setting.ini
	* \note
	*/
	bool loadSettings(QString _path = QString("./settings/setting.ini"));

	/** save the setting file
	*  
	* \note
	*/
	bool saveSettings(QString _file_name = QString("./settings/setting_save.ini"));

	void resetToDefaultValues();


private:

	QSettings *m_settings;

	int interpreteLanguage(QString _language);

	void getCOMsettings();

	void getSolutionSettings();

	void getGUIsettings();

	void getPRsettings();

	uint32_t giveRainbowColor(float _position);

	QString m_setting_file_name;

	QTranslator m_translator_tool;

protected:
	Ui::Labonatip_tools *ui_tools;    //!<  the user interface
};


#endif /* Labonatip_TOOLS_H_ */
