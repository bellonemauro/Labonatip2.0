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

#include "ui_tools.h"
#include <QMainWindow>
#include <QSettings>
#include <QTranslator>
#include <QDateTime>
#include <QMessageBox>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>

#include <dataStructures.h>

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
	void emptyWaste(); // generated when empty wells is pressed
	void refillSolution(); // generated when empty wells is pressed
	void colSol1Changed(const int _r, const int _g, const int _b);
	void colSol2Changed(const int _r, const int _g, const int _b);
	void colSol3Changed(const int _r, const int _g, const int _b);
	void colSol4Changed(const int _r, const int _g, const int _b);


public:

	explicit Labonatip_tools(QWidget *parent = 0);

	~Labonatip_tools();

	COMSettings getComSettings() { return *m_comSettings; }
	solutionsParams getSolutionsParams() { return *m_solutionParams; }
	pr_params getPr_params() { return *m_pr_params; }
	GUIparams getGUIparams() { return *m_GUI_params; }


	int language; //TODO add an enumerator

	void switchLanguage(QString _translation_file);

	void updateDevices() { this->enumerate(); }

	bool setLoadSettingsFileName(QString _filename) { 
		m_setting_file_name = _filename; 
		return loadSettings(m_setting_file_name);
	}

	bool setFileNameAndSaveSettings(QString _filename) {
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


	void emptyWastePressed();

	void refillSolutionPressed();

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

	void initCustomStrings();

	int interpreteLanguage(QString _language);

	void getCOMsettingsFromGUI();

	void getSolutionSettingsFromGUI();

	void getGUIsettingsFromGUI();

	void getPRsettingsFromGUI();

	uint32_t giveRainbowColor(float _position);

	QString m_setting_file_name;

	QTranslator m_translator_tool;


	COMSettings *m_comSettings;
	solutionsParams *m_solutionParams;
	pr_params *m_pr_params;
	GUIparams *m_GUI_params;

	// translatable strings
	QString m_str_warning;
	QString m_str_factory_reset;

protected:
	Ui::Labonatip_tools *ui_tools;    //!<  the user interface
};


#endif /* Labonatip_TOOLS_H_ */
