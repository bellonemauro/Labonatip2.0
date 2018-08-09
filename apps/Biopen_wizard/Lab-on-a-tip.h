/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_GUI_H_
#define Labonatip_GUI_H_

// standard libraries
#include <iostream>
#include <string>

// autogenerated form header
#include "ui_Lab-on-a-tip.h"

// Qt
#include <QMainWindow>
#include <QTranslator>
#include <QDateTime>
#include <QTimer>
//#include <QWhatsthis>
#include <qwhatsthis.h>
#include <QException>
#include <QtTextToSpeech/QTextToSpeech>
#include <QUndoView>
#include <QProgressDialog>
#include <QDir>
#include <QFileDialog>
#include <QMenu>
#include <QTextStream>

// QT for graphics
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>


#include "Q_DebugStream.h"
#include "tools.h"
//#include "protocolEditor.h"
#include "protocolReader.h"
#include "protocolWriter.h"
#include "protocolRunner.h"
#include "chart.h"

// serial
#include <serial/serial.h>
// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>


using namespace std;

// new shorter class name = Lab-on-a-tip  -- Loat 
class Labonatip_GUI : public QMainWindow
{
	Q_OBJECT

public:

	/** \brief Constructor, initialize objects and parameters using default values
	*
	*/
	explicit Labonatip_GUI(QMainWindow *parent = nullptr);

	/** \brief Destructor implementation to make sure everything is properly closed
	*
	*   Make sure the thread and the serial communication
	*   are properly closed, then free memory
	*/
	~Labonatip_GUI();
	
	/**  \brief Set the version of the software from the main 
	*
	*  @param _version  version to be assigned to the class member m_version
	*/
	void setVersion(string _version);

	/**  \brief Set the protocol path in the user folder
	*
	*  @param _path  path to be assigned to the data member m_protocol_path
	*/
	void setProtocolUserPath(QString _path);

	/**  \brief Set the settings path in the user folder
	*
	*  @param _path  path to be assigned to the data member m_settings_path
	*/
	void setSettingsUserPath(QString _path);

	/**  \brief Set the external data path in the user folder
	*
	*  @param _path  path to be assigned to the data member m_ext_data_path
	*
	*  \note This folder is used to save history and log or errors
	*/
	void setExtDataUserPath(QString _path) { m_ext_data_path = _path;  }

	/**  \brief Initial support for high-dpi screens
	*
	*  @param _dpiX 
	*  @param _dpiY
	*
	*  \note This function is still not fully working //TODO
	*/
	void appScaling(int _dpiX, int _dpiY);


private slots:

	void onProtocolClicked(QTreeWidgetItem *item, int column);
	void openProtocolFolder();
	void protocolsMenu(const QPoint & _pos);
	void deleteProtocol();
	void helpTriggered();
	void addCommand();
	void removeCommand();
	void moveUp();
	void moveDown();
	void plusIndent();
	bool itemChanged(QTreeWidgetItem *_item, int _column);
	void duplicateItem();
	void createNewLoop();
	void createNewLoop(int _loops);

	/** \brief Gets the clear commands request 
	*
	*  The command request is driven here to ask an "are you sure" message 
	*  to the user and then call the clearAllCommands function. 
	*  Other functions may use the clearAllCommands avoiding the message to pop out.
	*/
	void clearAllCommandsRequest();

	/** \brief Clear all the commands from the command tree in the editor 
	*/
	void clearAllCommands();
	
	/** \brief Visualize the stack for redo/undo currently hiden
	*/
	void showUndoStack();

	/** \brief Undo function for the command editor
	*/
	void undo();
	
	/** \brief Redo function for the command editor
	*/
	void redo();

	/** \brief Run a test for the TTL signals in the PPC1
	*
	*    In the tool dialog there is a button for TTL test, 
	*    the signal generated is catched here to run a test 
	*    on the TTL output of the PPC1. 
	*    If the PPC1 is not connected a message will pop out
	*/
	void testTTL(bool _state);

	/** \brief This function is called when the down arrow on Pon is called
	  *        it decreases the pressure on Pon, it does not accept out-of-range
	  */
	void pressurePonDown();

	/** \brief This function is called when the up arrow on Pon is called
	  *        it increases the pressure on Pon, it does not accept out-of-range
	  */
	void pressurePonUp();

	/** \brief This function is called when the down arrow on Poff is called
	  *        it decreases the pressure on Poff, it does not accept out-of-range
	  */
	void pressurePoffDown();

	/** \brief This function is called when the up arrow on Poff is called
	*          it increases the pressure on Poff, it does not accept out-of-range
	*/
	void pressurePoffUp();

	/** \brief This function is called when the down arrow on v_switch is called
	*          it decreases the vacuum, it does not accept out-of-range
	*/
	void pressButtonPressed_switchDown();

	/** \brief This function is called when the up arrow on v_switch is called
	*          it increases the vacuum, it does not accept out-of-range
	*/
	void pressButtonPressed_switchUp();

	/** \brief This function is called when the down arrow on v_recirc is called
	*          it decreases the vacuum, it does not accept out-of-range
	*/
	void recirculationDown();

	/** \brief This function is called when the up arrow on v_recirc is called
	*          it increases the vacuum, it does not accept out-of-range
	*/
	void recirculationUp();

	/** \brief Update macro status message
	*/
	void updateMacroStatusMessage(const QString &_message);

	/** \brief Update macro time status for the chart
	*/	
	void updateMacroTimeStatus(const double &_status);

	/** \brief Ask message
	*/
	void askMessage(const QString &_message);

	/** \brief Catch the signal from tool for the solution1 color changed
	*          to adapt the wells in the pipette to the new color  
	*/	
	void colSolution1Changed(const int _r, const int _g, const int _b);

	/** \brief Catch the signal from tool for the solution2 color changed
	*          to adapt the wells in the pipette to the new color
	*/
	void colSolution2Changed(const int _r, const int _g, const int _b);

	/** \brief Catch the signal from tool for the solution3 color changed
	*          to adapt the wells in the pipette to the new color
	*/
	void colSolution3Changed(const int _r, const int _g, const int _b);

	/** \brief Catch the signal from tool for the solution4 color changed
	*          to adapt the wells in the pipette to the new color
	*/
	void colSolution4Changed(const int _r, const int _g, const int _b);

	/** \brief Stop all pumps and close the valves
    */
	void pumpingOff();

	/** \brief Close the valves
	*/
	void closeAllValves();

	/** \brief pushSolution1
	*/
	void pushSolution1();

	/** \brief solution1
	*
	*   This receive the command from the protocol and
	*   simulate the solution 1 button press
	*/
	void solution1(bool _enable) {
		ui->pushButton_solution1->setChecked(_enable);
		pushSolution1();
	}

	/** \brief  pushSolution2
	*/
	void pushSolution2();

	/** \brief solution2
	*
	*   This receive the command from the protocol and
	*   simulate the solution 2 button press
	*/
	void solution2(bool _enable) {
		ui->pushButton_solution2->setChecked(_enable);
		pushSolution2();
	}

	/** \brief pushSolution3
	*/
	void pushSolution3();

	/** \brief solution3
	*
	*   This receives the command from the protocol and 
	*   simulate the solution 3 button press
	*/
	void solution3(bool _enable) {
		ui->pushButton_solution3->setChecked(_enable);
		pushSolution3();
	}

	/** \brief pushSolution4
	*
	*   //TODO: add an argument for all 1-4 _activate 
	*           if _activate = true, solution flow start
	*           if _activate = false, solution flow stop
	* \note
	*/
	void pushSolution4();

	/** \brief solution4
	*
	*   This receive the command from the protocol and
	*   simulate the solution 4 button press
	* \note
	*/
	void solution4(bool _enable) {
		ui->pushButton_solution4->setChecked(_enable);
		pushSolution4();
	}

	/** \brief Increase/reduce the area for the solution depiction
	*/
	void sliderPonChanged(int _value);

	/** \brief Set debug to terminal
	*/
	void dumpToTerminal(int _state) {
		qout->copyOutToTerminal(_state);  
		qerr->copyOutToTerminal(_state);
	};

	/** \brief Increase/reduce the area for the solution depiction
	*/
	void sliderPoffChanged(int _value);

	/** \brief Increase/reduce the area for the solution depiction
	*/
	void sliderRecircChanged(int _value);

	/** \brief Increase/reduce the area for the solution depiction
	*/
	void sliderSwitchChanged(int _value);

	/** \brief Update the GUI according to a timer
	*/
	void updateGUI();

	/** \brief Update the waste according to a timer
	*/
	void updateWaste();

	/** \brief This will give a remainder every 5 minutes to empty the waste
	*/
	void emptyWasteRemainder();

	/** \brief Recall the preset mode 1
	*
	*   The software allows 3 preset modes, that can be set according to 
	*   the current pressure/vacuum values that are memorized via resetPreset_n
	*   and recalled via setPreset_n
	*/
	void setPreset1();

	/** \brief Recall the preset mode 2
	*
	*   The software allows 3 preset modes, that can be set according to
	*   the current pressure/vacuum values that are memorized via resetPreset_n
	*   and recalled via setPreset_n
	*/
	void setPreset2();

	/** \brief Recall the preset mode 3
	*
	*   The software allows 3 preset modes, that can be set according to
	*   the current pressure/vacuum values that are memorized via resetPreset_n
	*   and recalled via setPreset_n
	*/
	void setPreset3();

	/** \brief Memorized the current value for the preset mode 1
	*
	*   The software allows 3 preset modes, that can be set according to
	*   the current pressure/vacuum values that are memorized via resetPreset_n
	*   and recalled via setPreset_n
	*/
	void resetPreset1();

	/** \brief Memorized the current value for the preset mode 2
	*
	*   The software allows 3 preset modes, that can be set according to
	*   the current pressure/vacuum values that are memorized via resetPreset_n
	*   and recalled via setPreset_n
	*/
	void resetPreset2();

	/** \brief Memorized the current value for the preset mode 3
	*
	*   The software allows 3 preset modes, that can be set according to
	*   the current pressure/vacuum values that are memorized via resetPreset_n
	*   and recalled via setPreset_n
	*/
	void resetPreset3();

	/** \brief This is supposed to be used from the solution release time to
	  * update the visualization of the circular sliders
	  */
	void updateTimingSliders( );

	/** \brief Visualize a message and a progress bar 
	*
	*  Very useful in the software, it handles all the progress messages
	*  visualizing the message _message for the time _seconds
	*
	*  @param _seconds time duration of the message in seconds
	*  @param _message string to be visualized
	*/
	bool visualizeProgressMessage(int _seconds, QString _message = " no message ");

	/** \brief  Enter what's this mode
	  */
	void  ewst();

	/** \brief Clear the history folder
	*
	* \note this is called every time the history user folder is over 10MB
	*/
	void cleanHistory();

	/** \brief Visualize the about dialog
	  */
	void  about();

	/** \brief Empty waste wells in the pipette
	*/
	void emptyWells();

	/** \brief Refill solution wells in the pipette
	*/
	void refillSolution();

	/** \brief Catch the ok signal from tool dialog
	*/
	void toolOk();

	/** \brief Catch the apply signal from tool dialog
	*/
	void toolApply();

	/** \brief This function is called when the button + on droplet size is clicked
	*
	*   only Pon and V_recirc + - 2.5%
	*/
	void dropletSizePlus();

	/** \brief This function is called when the button - on droplet size is clicked
	*
	*   only Pon and V_recirc + - 2.5%
	*/
	void dropletSizeMinus();


	/** \brief This function is called when the button + on flow speed is clicked
	*
	*   	 +5% to all values
	*	 Poff does not read too low values,
	*	 if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	*/
	void flowSpeedPlus();

	/** \brief This function is called when the button - on flow speed is clicked
	*
	*   	 -5% to all values
	*	 Poff does not read too low values,
	*	 if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	*/
	void flowSpeedMinus();

	/** \brief This function is called when the button + on flow speed is clicked
	*
	*   	 +5% v_recirculation
	*/
	void vacuumPlus();

	/** \brief This function is called when the button - on flow speed is clicked
	*
	*   	 -5% v_recirculation
	*/
	void vacuumMinus();

	/** \brief Update flow control percentages
	*
	*   Only in simulation recalculate the percentages 
	*   according to the same method in the PPC1 api
	*/
	void updateFlowControlPercentages();


	/** \brief Load icon pressed
	*
	*   When the load icon is pressed an automatic detection of GUI status
	*   commander or editor allows to call loadProtocol or loadSettings function
	*/
	void loadPressed();

	/** \brief Save icon pressed
	*
    *   When the save icon is pressed an automatic detection of GUI status
	*   commander or editor allows to call saveProtocol or saveSettings function
	*/
	void savePressed();

	/** \brief Show the tool dialog
	*/
	void showToolsDialog();

	/** \brief Show the protocol editor
	*/	
	void showProtocolEditor();

	/** \brief Enter simulation mode - the PPC1 will not be used
	*/
	void simulationOnly();

	/** \brief Connect and disconnect the PPC1
	*
	*   \param _connect = true to connect, false to disconnect
	*/
	bool disCon(bool _connect);
	
	/** \brief Reboot the PPC1
	*
	* \note This is a hard reboot for the PPC1 device,
	*       it will cause the disconnection from the serial port
	*/
	void reboot();

	/** \brief Run the shutdown procedure
	*
	* \note
	*/
	void shutdown();

	/** \brief Open/close the dock for advanced tools
	*
	* \note
	*/
	void closeOpenDockTools();

	/** \brief Resize the top toolbar
	*
	*  On resize event it changes the size 
	*  of the toolbar to properly visualize the advanced/basic icon
	*/
	void resizeToolbar();

	/** \brief The operation run in background, a signal is emitted at the end
	*
	*/
	void newTip();

	/** \brief The operation run in background, a signal is emitted at the end
	*
	*/
	void runProtocol();

	/** \brief Catch the end signal from runProtocol
	*
	*/
	void protocolFinished(const QString &_result);

	/** \brief Run --- still work in progress
	*
	*    It will put the device into the operational mode by running the following macro
	*      allOff()
	*      setPoff(21)
	*      setPon(190)
	*      setVswitch(-115)
	*      setVrecirc(-115)
	*
	*
	* \note
	*/
	void operationalMode();

	/** \brief Stop flow macro running
	*
	*  	  AllOff()
	*  	  setPoff(0)
	* 	  setPon(0)
	*	  sleep(3)
	*	  setVswitch(0)
	*	  setVrecirc(0)
	*	  sleep(3)
	*
	* \note
	*/
	void stopFlow();

	/** \brief Stop solution flow
	*
	*   Stop the flow of the solution if one of the solutions is flowing
	*
	*
	* \note
	*/
	void stopSolutionFlow();

	/** \brief  Put the device into a standby mode
	*
	*       Put the device into a standby mode by running the following commands:
	*
	*       STANDBY MACRO
	*       allOff()
	*       setPoff(11)
	*       setPon(0)
	*       sleep(5)
	*       setVswitch(-45)
	*       setVrecirc(-45)
	*/
	void standby();

protected:
// event control

	/** \brief The close event is triggered to pass through the destructor
	*/
	void closeEvent(QCloseEvent *event);
	
	/** \brief Allows to resize the toolbar triggering the change gui size event
	*/
	void changeEvent(QEvent*);

	/** \brief All the events are triggered and filtered 
	*
	*     This will allow to activate and deactivate the tool tips
	*/
	bool eventFilter(QObject *_obj, QEvent *_event);

private:

	/** \brief Update pon set point
	*
	*     This function is called when the up/down arrow on Pon is pressed, but also
	*     when the Pon slider is changed and every time the update of the value
	*     for the pressurized channel Pon is needed.
	*
	*  @param _pon_set_point new set point value
	*
	* \note the value is updated in simulation or directly to the PPC1 if connected and running
	*/
	void updatePonSetPoint(double _pon_set_point);

	/** \brief Update poff set point
	*
	*     This function is called when the up/down arrow on Poff is pressed, but also
	*     when the Poff slider is changed and every time the update of the value
	*     for the pressurized channel Poff is needed.
	*
	*  @param _poff_set_point new set point value
	*
	* \note the value is updated in simulation or directly to the PPC1 if connected and running
	*/
	void updatePoffSetPoint(double _poff_set_point);

	/** \brief Update vacuum recirculation set point
	*
	*     This function is called when the up/down arrow on recirculation channel is pressed,
	*     but also when the recirculation slider is changed and every time the update
	*     of the value for the vacuum channel is needed.
	*
	*  @param _v_recirc_set_point new set point value
	*
	* \note the value is updated in simulation or directly to the PPC1 if connected and running
	*/
	void updateVrecircSetPoint(double _v_recirc_set_point);

	/** \brief Update vacuum switch set point
	*
	*     This function is called when the up/down arrow on switch channel is pressed,
	*     but also when the recirculation slider is changed and every time the update
	*     of the value for the vacuum channel is needed.
	*
	*  @param _v_switch_set_point new set point value
	*
	* \note the value is updated in simulation or directly to the PPC1 if connected and running
	*/
	void updateVswitchSetPoint(double _v_switch_set_point);

	/** \brief Generate a style sheet to change the weels drawing in the pipette
	*
	*  @param _r  red
	*  @param _g  green
	*  @param _b  blue
	*
	*  \return a string containing a style sheet
	*/
	QString generateStyleSheet(const int _r, const int _g, const int _b);

	/** \brief Overload of generate a style sheet to change the weels drawing in the pipette
	*
	*  @param _color 
	*
	*  \return a string containing a style sheet
	*/
	QString generateStyleSheet(const QColor _color) {
		return generateStyleSheet(_color.red(), _color.green(), _color.blue());
	}

	/** \brief Dump logs to file, including messages from the console etc
	*/
	void dumpLogs();

  /** \brief Increase/reduce the area for the solution drawing
  *  
  *  @param _value  from 0 to MAX_ZONE_SIZE_PERC
  *
  * \note _value = 0 makes the flow to disappear
  */
  void updateDrawing(int _value);

  /** \brief set status led to connect or disconnect
  *
  *  @param _connect false = red, true = green
  */
  void setStatusLed( bool _connect = false );

  /** \brief group all the connects from gui objects to functions
  *
  */
  void initConnects();

  /** \brief Initialize the custom strings to allow translations
  *
  */
  void initCustomStrings();

  /** \brief Adds to the tree widget all the protocols that are found in the folder
  *
  *  @param  _path folder path
  */
  void readProtocolFolder(QString _path);

  /** \brief All the commands in the tree widget are added to the protocol
  *
  */
  void addAllCommandsToProtocol();

  /** \brief Calculate the protocol duration in seconds
  *
  *   Calculate the protocol duration in seconds by summing 
  *   the waiting commands in a protocol
  *
  *  @param _protocol a protocol as a list of PPC1api commands
  *
  * \return a double with the protocol duration in seconds
  */
  double protocolDuration(std::vector<fluicell::PPC1api::command> _protocol);

  /** \brief enable/disable the entire main window
  *
  *  @param _enable true or false
  */
  void setEnableMainWindow(bool _enable = false);

  /** \brief enable/disable the solution buttons 
  *
  *   Enable or disable the solution button, it is used for instance during a 
  *   protocol running to prevent the user to apply commands
  *
  *  @param _enable true or false
  */
  void setEnableSolutionButtons(bool _enable = false);

  /** \brief enable/disable the tab page 2 in the advanced panel
  *
  *   Make sure that enabling/disabling the tab page 2 in the advanced panel
  *   all the objects are disabled/enabled except for the leds as they have to
  *   be always enabled to avoid them to get greyed out
  *
  *  @param _enable true or false
  */
  void enableTab2(bool _enable);

  /** \brief Switch the language in the GUI
  *
  *  @param _value is the index of the language to load
  */
  void switchLanguage(int _value);

  /** \brief Open a setting file from a user folder
  */
  void openSettingsFile();

  /** \brief Load a protocol file
  */
  bool loadProtocol();

  /** \brief save the settings to a file
  *
  *   Save the current settings to a .ini file in the user folder
  */
  void saveSettingsFile();

  /** \brief Save the current protocol to file
  *
  *   \note the current protocol file is overriden
  */
  bool saveProtocol();

  /** \brief Save the current protocol to file 
  *
  *   \note a dialog will ask the user to define the protocol name
  */
  bool saveProtocolAs();

  /** \brief Update solution flow values (including calculation)
  */
  void updateFlows();

  /** \brief Update pressure and vacuum values
  */
  void updatePressureVacuum();

  /** \brief Update solution flow values
  */
  void updateSolutions();

  /** \brief Update the leds of pressures and vacuum
  */
  void updatePPC1Leds();

  /** \brief Safe triggering of PPC1api exceptions when the PPC1 is active
  */
  bool isExceptionTriggered();

// Class members
  Ui::Labonatip_GUI *ui;               //!< the main user interface
  Labonatip_tools * m_dialog_tools;    //!< pointer to the tools dialog

  QDebugStream *qout;                 //!< redirect cout for messages into the GUI
  QDebugStream *qerr;                 //!< redirect cerr for messages into the GUI

  //settings
  COMSettings *m_comSettings;         //!< communication settings
  solutionsParams *m_solutionParams;  //!< solution parameters, names a default values
  pr_params *m_pr_params;             //!< pressure and vacuum parameters
  GUIparams *m_GUI_params;            //!< GUI parameters

  pipetteStatus *m_pipette_status;    //!< pipette status in terms of pressures, vacuum and flows

  // for serial communication with PPC1 API
  fluicell::PPC1api *m_ppc1;  //!< object for the PPC1api connection
  std::vector<fluicell::PPC1api::command> *m_protocol;   //!< this is the current protocol to run

  bool m_pipette_active;    //!< true when the pipette is active and communicating, false otherwise
  bool m_simulationOnly;    //!< if active the software will run without the hardware device connected
  
  // Threding for protocols and update GUI
  Labonatip_macroRunner *m_macroRunner_thread;
  QTimer *m_update_flowing_sliders;        //!< connected to an update visualization function relative to solutions flow
  QTimer *m_update_GUI;                    //!< update GUI to show PPC1 values
  QTimer *m_update_waste;                  //!< update GUI to show PPC1 values
  QTimer *m_waste_remainder;               //!< empty waste remainder every 5 minutes
  const int m_base_time_step;              //!< used to set the update timers, every step is by default 1000 ms
  int m_flowing_solution;                  //!< needed for the visualization function relative to solution 1 - 2 - 3- 4

  //!< set the multiplicators for the time step, 
  //    e.g. desired_duration (sec) = multiplicator * m_base_time_step (100ms)
  //    TODO: KNOWN ISSUE: timing is not well fulfilled --- it is longer than expected
  int m_time_multipilcator;           //!< used to set the update time for the timers
  int m_timer_solution;               //!< duration of injection for solution 
  double m_protocol_duration;         //!< this is the timeline for the protocol execution
  QString m_current_protocol_file_path;
  QString m_current_protocol_file_name;

  //GUI stuff for drawing solution flow, remember to NEVER change this values manually, 
  // there is a hiden tools for the regulation of the flow drawing in the tabWidget panel
  // the hiden tool is visualized only in debug, see class constructor
  QGraphicsScene *m_scene_solution;   //!< scene to draw the solution flow
  QPen m_pen_line;                    //!< pen to draw the solution inside the pipe
  const int m_pen_line_width;         //!< pen line width, value = 5
  const float l_x1;                   //!< x-coordinate of the line starting point, value = -18.0
  const float l_y1;                   //!< y-coordinate of the line starting point, value = 49.0  
  const float l_x2;                   //!< x-coordinate of the line ending point, value = 55.0  
  const float l_y2;                   //!< y-coordinate of the line ending point, value = l_y1

  QColor m_sol1_color;      //!< my solution 1 color for the pipette drawing
  QColor m_sol2_color;      //!< my solution 2 color for the pipette drawing
  QColor m_sol3_color;      //!< my solution 3 color for the pipette drawing
  QColor m_sol4_color;      //!< my solution 4 color for the pipette drawing

  // chart
  QtCharts::QChartView *m_chartView;
  Labonatip_chart *m_labonatip_chart_view;   //!< the main chart is built in a different class

  // zone controls
  double m_ds_perc;          //!< droplet size percentage
  double m_fs_perc;          //!< flow speed percentage
  double m_v_perc;           //!< vacuum percentage

  QString m_version;         //!< software version
  QString m_protocol_path;   //!< protocol path 
  QString m_settings_path;   //!< settings path
  QString m_ext_data_path;   //!< ext data path (save history)
  QTranslator m_translator;  //!< translator object
  int m_language_idx;        //!< language index 1 = english

  int m_cmd_idx_c;       //!< index of the column for command index
  int m_cmd_command_c;   //!< index of the column for the command
  int m_cmd_range_c;     //!< index of the column for the range
  int m_cmd_value_c;     //!< index of the column for the value
  int m_cmd_msg_c;       //!< index of the column for the command status message
  int m_cmd_level_c;     //!< index of the column for the level in the tree

  // the delegates allows the protocol tree widget to have customized fields
  ComboBoxDelegate * m_combo_delegate;
  NoEditDelegate * m_no_edit_delegate;
  NoEditDelegate * m_no_edit_delegate2;
  SpinBoxDelegate * m_spinbox_delegate;

  // this is used only to pass the row index
  // from the popup menu into the tree widget
  // to the delete_protocol function
  int m_triggered_protocol_item;  //!< triggered row with the right click in the protocol list
  
  //object for reading and writing protocols
  Labonatip_protocolReader *m_reader;
  Labonatip_protocolWriter *m_writer;

  // for undo
  QUndoStack *m_undo_stack;
  QUndoView *m_undo_view;

  // custom strings for translations
  QString m_str_areyousure;
  QString m_str_waiting;
  QString m_str_advanced;
  QString m_str_basic;
  QString m_str_operation_cancelled;
  QString m_str_no_file_loaded;
  QString m_str_information;
  QString m_str_warning;
  QString m_str_error;
  QString m_str_cancel;
  QString m_str_ok;
  QString m_str_save;
  QString m_str_load;
  QString m_str_commander;
  QString m_str_editor;
  QString m_str_PPC1_status_con;
  QString m_str_PPC1_status_discon;
  QString m_str_protocol_running;
  QString m_str_protocol_not_running;
  QString m_str_connect;
  QString m_str_disconnect;
  QString m_str_save_profile;
  QString m_str_load_profile;
  QString m_str_cannot_save_profile;
  QString m_str_cannot_load_profile;
  QString m_str_warning_simulation_only;
  QString m_str_cannot_connect_ppc1;
  QString m_str_cannot_connect_ppc1_twice;
  QString m_str_cannot_connect_ppc1_check_cables; 
  QString m_str_question_find_device;
  QString m_str_ppc1_connected_but_not_running;
  QString m_str_question_stop_ppc1;
  QString m_str_unable_stop_ppc1;
  QString m_str_shutdown_pressed;
  QString m_str_shutdown_pressed_p_off;
  QString m_str_shutdown_pressed_v_off;
  QString m_str_rebooting;
  QString m_str_reconnecting;
  QString m_str_initialization; 
  QString m_str_newtip_msg1;
  QString m_str_newtip_msg2;
  QString m_str_newtip_msg3;
  QString m_str_newtip_msg4;
  QString m_str_newtip_msg5;
  QString m_str_newtip_msg6;
  QString m_str_newtip_msg7;
  QString m_str_newtip_msg8;
  QString m_str_newtip_msg9;
  QString m_str_newtip_msg10;
  QString m_str_stop_1;
  QString m_str_stop_2;
  QString m_str_standby_operation;
  QString m_str_label_run_protocol;
  QString m_str_label_stop_protocol;
  QString m_str_no_protocol_load_first;
  QString m_str_loaded_protocol_is;
  QString m_str_protocol_confirm;
  QString m_str_progress_msg1;
  QString m_str_progress_msg2;
  QString m_str_ask_msg;
  QString m_str_editor_apply_msg1;
  QString m_str_editor_apply_msg2;
  QString m_str_cleaning_history_msg1;
  QString m_str_cleaning_history_msg2;
  QString m_str_update_time_macro_msg1;
  QString m_str_update_time_macro_msg2;
  QString m_str_pulse_remaining_time;
  QString m_str_pulse_continuous_flowing;
  QString m_str_pulse_waste;
  QString m_str_pulse_full_in;
  QString m_str_operation_cannot_be_done; 
  QString m_str_out_of_bound; 
  QString m_str_user;
  QString m_str_protocol_running_stop;
  QString m_str_lost_connection;
  QString m_str_swapping_to_simulation;
  QString m_str_warning_solution_end;
  QString m_str_warning_waste_full;
  QString m_str_add_protocol_bottom;
  QString m_str_add_protocol_bottom_guide;
  QString m_str_protocol_duration;
  QString m_str_select_folder;
  QString m_str_remove_file;
  QString m_str_current_prot_name;
  QString m_str_question_override;
  QString m_str_override_guide;
  QString m_str_file_not_saved;
  QString m_str_save_protocol;
  QString m_str_clear_commands;
  QString m_str_solution_ended;
  QString m_str_waste_full;
  QString m_str_TTL_failed;

  // speech synthesis
  QTextToSpeech *m_speech;
  QVector<QVoice> m_voices;


  // to visualize the led on the status bar, 
  // I create two different leds and switch between them to create the effect on/off
  // the same led objects are used in different places
  QPixmap * led_green;
  QPixmap * led_orange;
  QPixmap * led_red;
  QPainter * painter_led_green;
  QPainter * painter_led_orange;
  QPainter * painter_led_red;

  // spacers for the toolbar to create the basic->advanced effect
  QGroupBox * m_g_spacer;
  QAction *m_a_spacer;

  const QString display_e = "E";
};

#endif /* Labonatip_GUI_H_ */
