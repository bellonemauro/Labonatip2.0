/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
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
#include <QDir>

#include <serial/serial.h>

#include <dataStructures.h>

using namespace std;


/** \brief Labonatip_tools class for settings
*
*   Tools open a new window to allow the user to choose settings. 
*   There are 4 main subsection:
*      - General settings : 
*      - Solution settings :
*      - Pressure and vacuum settings :
*      - Communication settings :
*
*   For each of this section a data structure is defined in the 
*   header dataStructure.h
*
*/
class Labonatip_tools : public  QMainWindow
{
	Q_OBJECT

	/** Create signals to be passed to the main app,
	*   the signals allows the tools class to send information to the main class for specific events
	*   the main class implements a connect to a slot to handle the emitted signals
	* 
	*/
	signals :
		void apply(); //!< signal generated when apply is pressed
		void cancel(); //!< signal generated when cancel is pressed
		void emptyWaste(); //!< signal generated when empty wells is pressed
		void refillSolution(); //!< signal generated when empty wells is pressed
		void TTLsignal(bool _state); //!< signal generated test TTL is pressed
		void colSol1Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol2Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol3Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void colSol4Changed(const int _r, const int _g, const int _b); //!< signal generated when the solution color is changed
		void checkUpdatesNow(); //!< signal generated when the updates button is pressed

public:

	explicit Labonatip_tools(QWidget *parent = 0); //!< Explicit ctor

	~Labonatip_tools(); //!< dtor

	/** \brief Get communication settings
	*
	*   Expose to the main class the communication settings
	*
	*   \return COMSettings
	*/
	COMSettings getComSettings() { return *m_comSettings; }

	/** \brief Get solution settings
	*
	*   Expose to the main class the solution settings
	*
	*   \return solutionsParams
	*/
	solutionsParams getSolutionsParams() { return *m_solutionParams; }

	/** \brief Get pressure and vacuum settings
	*
	*   Expose to the main class the pressure and vacuum settings
	*
	*   \return pr_params
	*/
	pr_params getPr_params() { return *m_pr_params; }

	/** \brief Get GUI settings
	*
	*   Expose to the main class the GUI settings
	*
	*   \return GUIparams
	*/
	GUIparams getGUIparams() { return *m_GUI_params; }

	void switchLanguage(QString _translation_file);

	void updateDevices() { this->enumerate(); 
	this->applyPressed();
	}

	bool setLoadSettingsFileName(QString _filename) { 
		m_setting_file_name = _filename; 
		return loadSettings(m_setting_file_name);
	}

	void setExtDataPath(QString _filename) {
		m_GUI_params->outFilePath = _filename;
		ui_tools->lineEdit_msg_out_file_path->setText(_filename);
	}

	bool setFileNameAndSaveSettings(QString _filename) {
		m_setting_file_name = _filename;
		return saveSettings(_filename);
	}

	void setDefaultPressuresVacuums(int _p_on_default, int _p_off_default, 
		                            int _v_recirc_default, int _v_switch_default);

	/** \brief Set values of preset group 1
	*
	* \note
	*/
	void setPreset1(int _p_on, int _p_off, int _v_switch, int _v_recirc);

	/** \brief Set values of preset group 2
	*
	* \note
	*/
	void setPreset2(int _p_on, int _p_off, int _v_switch, int _v_recirc);

	/** \brief Set values of preset group 3
	*
	* \note
	*/
	void setPreset3(int _p_on, int _p_off, int _v_switch, int _v_recirc);

	QString getUserName() { return ui_tools->lineEdit_userName->text(); }

private slots:

	void goToPage1();
	void goToPage2();
	void goToPage3();
	void goToPage4();

	/** Show information for the serial port communication
	*
	* \note
	*/
	void showPortInfo(int idx);

	void enableToolTip(int _inx);

	void checkForUpdates();

	void testTTL();
	/** Color solution 1 changed
	*
	* \note
	*/
	void colorSol1Changed(int _value);

	/** Color solution 2 changed
	*
	* \note
	*/
	void colorSol2Changed(int _value);

	/** Color solution 3 changed
	*
	* \note
	*/
	void colorSol3Changed(int _value);

	/** Color solution 4 changed
	*
	* \note
	*/
	void colorSol4Changed(int _value);

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s1(int _state) {
		  ui_tools->doubleSpinBox_pulse_sol1->setEnabled(!_state); 
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s2(int _state) {
		ui_tools->doubleSpinBox_pulse_sol2->setEnabled(!_state); 
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s3(int _state) {
		ui_tools->doubleSpinBox_pulse_sol3->setEnabled(!_state); 
	}

	/** \brief Enable/Disable the timer for an infinite solution flow
	*
	* \note
	*/
    void setContinuousFlow_s4(int _state) {
		ui_tools->doubleSpinBox_pulse_sol4->setEnabled(!_state); 
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

	/** emit empty waste signal on button pressed
	*
	*/
	void emptyWastePressed();

	/** emit empty refill solution signal on button pressed
	*
	*/
	void refillSolutionPressed();

	/** Enumerate serial ports
	*
	*/
	void enumerate();

	/** on enable check box clicked, it enable/disable the filtering
	*
	*/
	void enablePPC1filtering() {
		ui_tools->spinBox_PPC1filterSize->setEnabled(
			ui_tools->checkBox_enablePPC1filter->isChecked());
	}

	/** Load an ini setting file
	* in the GUI initialization it takes a default value ./settings/setting.ini
	* \note
	*/
	bool loadSettings(QString _path = QString("./settings/setting.ini"));

	/** Save the setting file
	*  
	* \note 
	*/
	bool saveSettings(QString _file_name = QString("./settings/setting_save.ini"));

	void resetToDefaultValues();

private:

	QSettings *m_settings;

	void initCustomStrings();

	int parseLanguageString(QString _language);

	void getCOMsettingsFromGUI();

	void getSolutionSettingsFromGUI();

	void getGUIsettingsFromGUI();

	void getPRsettingsFromGUI();

	uint32_t giveRainbowColor(float _position);

    /** \brief Check the size of the history and ask to clean it if necessary
    *
    * \note
    */
    void checkHistory ();

    int calculateFolderSize(const QString _wantedDirPath);

	QString m_setting_file_name;

	QTranslator m_translator_tool;

	COMSettings *m_comSettings;
	solutionsParams *m_solutionParams;
	pr_params *m_pr_params;
	GUIparams *m_GUI_params;

	// translatable strings
	QString m_str_warning;
	QString m_str_factory_reset;
	QString m_str_areyousure;
	QString m_str_information; 
	QString m_str_ok;
	QString m_str_operation_cancelled;
	QString m_str_history_cleaned;

protected:
	Ui::Labonatip_tools *ui_tools;    //!<  the user interface
};


#endif /* Labonatip_TOOLS_H_ */
