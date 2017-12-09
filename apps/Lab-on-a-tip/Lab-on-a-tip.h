/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_GUI_H_
#define Labonatip_GUI_H_

// standard libraries
#include <iostream>
#include <string>
//#include <math.h>
#include <fstream>
// Qt
#include <QApplication>
#include <QMainWindow>
#include <QTranslator>
#include <QToolButton>
#include <QDialog>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include "ui_Lab-on-a-tip.h"
#include "ui_Lab-on-a-tip_tools.h"
#include "ui_labonatip_macroWizard.h"
#include "ui_Lab-on-a-tip_protocolEditor.h"

#include <QTextStream>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QStringList>
#include <QProgressDialog>
#include <QDir>
#include <QDateTime>
#include <QTimer>
//#include <QWhatsthis>
#include <qwhatsthis.h>

// QT for graphics
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>

#include "Q_DebugStream.h"
#include "Lab-on-a-tip_tools.h"
#include "Lab-on-a-tip_protocolEditor.h"
#include "Lab-on-a-tip_macroRunner.h"
#include "Lab-on-a-tip_macroWizard.h"
#include "Lab-on-a-tip_chart.h"


// serial
#include <serial/serial.h>
// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>


using namespace std;

class Labonatip_chart;
class Labonatip_macroRunner;
class Labonatip_GUI;
class Labonatip_tools;
class Labonatip_protocol_editor;
class Labonatip_macroWizard;

// new shorter class name = Lab-on-a-tip  -- Loat 
class Labonatip_GUI : public QMainWindow
{
	Q_OBJECT

#define LENGTH_TO_TIP 0.065
#define LENGTH_TO_ZONE 0.062

public:
	explicit Labonatip_GUI(QMainWindow *parent = nullptr);

	~Labonatip_GUI();
	

	/**  Set the version of the software from the main 
	*
	*/
	void setVersion(string _version);


	void setMacroUserPath(QString _path) { m_macro_path = _path; }

	void setSettingsUserPath(QString _path) { m_settings_path = _path; }

	void setExtDataUserPath(QString _path) { m_ext_data_path = _path; }

	/** \brief This function is called when the down arrow on Pon is called
	*        it decreases the pressure on Pon, it does not accept out-of-range
	*
	* \note
	*/
	void updatePonSetPoint(double _pon_set_point);

	/** \brief This function is called when the down arrow on Pon is called
	*        it decreases the pressure on Pon, it does not accept out-of-range
	*
	* \note
	*/
	void updatePoffSetPoint(double _poff_set_point);

	/** \brief This function is called when the down arrow on Pon is called
	*        it decreases the pressure on Pon, it does not accept out-of-range
	*
	* \note
	*/
	void updateVrecircSetPoint(double _v_recirc_set_point);

	/** \brief This function is called when the down arrow on Pon is called
	*        it decreases the pressure on Pon, it does not accept out-of-range
	*
	* \note
	*/
	void updateVswitchSetPoint(double _v_switch_set_point);

private slots:



	/** \brief This function is called when the down arrow on Pon is called
	  *        it decreases the pressure on Pon, it does not accept out-of-range
	  *
	  * \note
	  */
	void pressurePonDown();

	/** \brief This function is called when the up arrow on Pon is called
	  *        it increases the pressure on Pon, it does not accept out-of-range
	  *
	  * \note
	  */
	void pressurePonUp();

	/** \brief This function is called when the down arrow on Poff is called
	  *        it decreases the pressure on Poff, it does not accept out-of-range
	  *
	  * \note
	  */
	void pressurePoffDown();

	/** \brief This function is called when the up arrow on Poff is called
	*          it increases the pressure on Poff, it does not accept out-of-range
	*
	* \note
	*/
	void pressurePoffUp();

	/** \brief This function is called when the down arrow on v_switch is called
	*          it decreases the vacuum, it does not accept out-of-range
	*
	* \note
	*/
	void pressButtonPressed_switchDown();

	/** \brief This function is called when the up arrow on v_switch is called
	*          it increases the vacuum, it does not accept out-of-range
	*
	* \note
	*/
	void pressButtonPressed_switchUp();


	/** \brief This function is called when the down arrow on v_recirc is called
	*          it decreases the vacuum, it does not accept out-of-range
	*
	* \note
	*/
	void recirculationDown();

	/** \brief This function is called when the up arrow on v_recirc is called
	*          it increases the vacuum, it does not accept out-of-range
	*
	* \note
	*/
	void recirculationUp();


	/** \brief Update macro status message
	*
	* \note
	*/
	void updateMacroStatusMessage(const QString &_message);

	/** \brief Update macro time status for the chart
	*
	* \note
	*/	
	void updateMacroTimeStatus(const double &_status);

	/** \brief Ask message
	*
	* \note
	*/
	void askMessage(const QString &_message);

	void colSolution1Changed(const int _r, const int _g, const int _b);

	void colSolution2Changed(const int _r, const int _g, const int _b);

	void colSolution3Changed(const int _r, const int _g, const int _b);

	void colSolution4Changed(const int _r, const int _g, const int _b);

	/** \brief Stop all pumps and close the valves
	   *
	   * \note
	   */
	void pumpingOff();

	/** \brief Close the valves
	*
	* \note
	*/
	void closeAllValves();


	/** \brief pushSolution1
	*
	* \note
	*/
	void pushSolution1();

	/** \brief  pushSolution2
	*
	* \note
	*/
	void pushSolution2();

	/** \brief pushSolution3
	*
	* \note
	*/
	void pushSolution3();

	/** \brief pushSolution4
	*
	* \note
	*/
	void pushSolution4();

	/** \brief Set current pressure/vacuum values as 100% default values
	*
	* \note
	*/
	void setAsDefault();

	/** \brief Put to zero the indicators
	*
	* \note
	*/
	void resetWells();

	/** \brief Increase/reduce the area for the solution depiction
	*
	* \note
	*/
	void sliderPonChanged(int _value);


	void switchLanguage(int _value);


	/** \brief Set debug to terminal
	*
	* \note
	*/
	void dumpToTerminal(int _state) {
		qout->copyOutToTerminal(_state);  
		qerr->copyOutToTerminal(_state);
	};


	/** \brief Set debug to terminal
	*
	* \note
	*/
	void setPpc1Verbose(int _state) {
		m_ppc1->setVebose(_state);
	};

	/** \brief Increase/reduce the area for the solution depiction
	*
	* \note
	*/
	void sliderPoffChanged(int _value);

	/** \brief Increase/reduce the area for the solution depiction
	*
	* \note
	*/
	void sliderRecircChanged(int _value);

	/** Increase/reduce the area for the solution depiction
	*
	* \note
	*/
	void sliderSwitchChanged(int _value);

	/** \brief This is supposed to be used from the solution release time to
	  * update the visualization of the circular sliders
	  *
	  * \note
	  */
	void updateTimingSliders( );

	void updateFlows();

	void updateGUI();

	/** \brief  Set the sLog file to be saved true = save // false = not save
	  *
	  *  \param  _Log_file_name   = file name
	  *  /note -
	  **/
	//void setLogFile(QString &_Log_file_name) {
	//	Log_file_name = _Log_file_name;
	//};

	/** \brief save log file
	  *
	  * /return false for any writing error
	  **/
	//bool saveLog(QString &_file_name);


	/** \brief Visualize a message and a progress bar 
	* \note
	*/
	bool visualizeProgressMessage(int _seconds, QString _message = " no message ");

	/** \brief  Enter what's this mode
	  * \note
	  */
	void  ewst();

	/** \brief Visualize the about dialog
	  * \note
	  */
	void  about();

	void cleanHistory();

	void toolEmptyWells();

	/** \brief Catch ok signal from tool dialog
	* \note
	*/
	void toolOk();

	/** \brief Catch apply signal from tool dialog
	* \note
	*/
	void toolApply();

	void editorOk();

	void editorApply();

//DELIVERY

	/** \brief This function is called when the button + on droplet size is clicked
	*
	*   only Pon and V_recirc + - 2.5%
	*
	* \note
	*/
	void dropletSizePlus();

	/** \brief This function is called when the button - on droplet size is clicked
	*
	*   only Pon and V_recirc + - 2.5%
	*
	* \note
	*/
	void dropletSizeMinus();


	/** \brief This function is called when the button + on flow speed is clicked
	*
	*   	 +5% to all values
	*	 Poff does not read too low values,
	*	 if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	*
	* \note
	*/
	void flowSpeedPlus();

	/** \brief This function is called when the button - on flow speed is clicked
	*
	*   	 -5% to all values
	*	 Poff does not read too low values,
	*	 if 5% different is less than 5 mbar .... start -> start + 5 --> start - 5%
	*
	* \note
	*/
	void flowSpeedMinus();

	/** \brief This function is called when the button + on flow speed is clicked
	*
	*   	 +5% v_recirculation
	*
	* \note
	*/
	void vacuumPlus();

	/** \brief This function is called when the button - on flow speed is clicked
	*
	*   	 -5% v_recirculation
	*
	* \note
	*/
	void vacuumMinus();

	/** \brief Update flow control percentages
	*
	*   Only in simulation recalculate the percentages according to the same method in the PPC1 api
	*
	* \note
	*/
	void updateFlowControlPercentages();


//END DELIVERY


//TOOLSACTIONS

	/** \brief Open a setting file
	* 
	*   Open a setting file from a user folder
	*  
	* \note
	*/
	void openFile();

	/** \brief save the settings to a file
	*
	*   Save the current settings to a .ini file in the user folder
	*
	* \note
	*/
	void saveFile();

	/** \brief This function shows a tool dialog,
	*        all the settings must be implemented here
	*
	* \note
	*/
	void showToolsDialog();


	/** \brief This function shows a protocol editor dialog,
	*       
	*
	* \note
	*/	void showProtocolEditorDialog();


	/** \brief Enter simulation mode - the PPC1 will not be used
	* \note
	*/
	void simulationOnly();

	/** \brief Connect and disconnect the PPC1
	*
	* \note
	*/
	void disCon();
	
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

	/** \brief Open/close the dock for advaced tools
	*
	* \note
	*/
	void closeOpenDockTools();

	void resizeToolbar();

//END TOOLSACTIONS

//PPC1ACTIONS:

	/** \brief The operation run in background, a signal is emitted at the end
	*
	* \note
	*/
	void newTip();


	/** \brief The operation run in background, a signal is emitted at the end
	*
	* \note
	*/
	void runMacro();


	/** \brief The operation run in background, a signal is emitted at the end
	*
	* \note
	*/
	void macroFinished(const QString &_result);

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
	*       Put the device into a standy mode by running the following commands:
	*
	*       STANDBY MACRO
	*       allOff()
	*       setPoff(11)
	*       setPon(0)
	*       sleep(5)
	*       setVswitch(-45)
	*       setVrecirc(-45)
	*
	* \note
	*
	*/
	void standby();

//END PPC1ACTIONS

protected:
	// event control, so far only translations are handled
	void closeEvent(QCloseEvent *event); 
	
	void changeEvent(QEvent*);

	void resizeEvent(QResizeEvent*);

	bool eventFilter(QObject *_obj, QEvent *_event);

private:

	void setEnableMainWindow(bool _enable = false);

	void setEnableSolutionButtons(bool _enable = false);

	QString generateStyleSheet(const QColor _color) {
		return generateStyleSheet(_color.red(), _color.green(), _color.blue());
	}

	QString generateStyleSheet(const int _r, const int _g, const int _b);

	// save log data, messages from the console ect. 
	void dumpLogs();

  /** Increase/reduce the area for the solution depiction
  *  
  * \note
  */
  void updateDrawing(int _value);

  /** set status led to connect or disconnect
  *
  */
  void setStatusLed( bool _connect = false );

  /** group all the connects are in this function
  *
  */
  void initConnects();

  Ui::Labonatip_GUI *ui;    //!< the user interface



  QDebugStream *qout;            //--> redirect for messages into the GUI
  QDebugStream *qerr;            //--> redirect for messages into the GUI

  Labonatip_tools * m_dialog_tools; //!< pointer to the tools dialog

  Labonatip_protocol_editor * m_dialog_p_editor; //!< pointer to the tools dialog

  QGraphicsScene *m_scene_solution;   //!< scene to draw the solution flow
  
  // for serial communication
  QLabel *status;

  fluicell::PPC1api *m_ppc1;  //!< object for the PPC1api connection
  std::vector<fluicell::PPC1api::command> *m_macro;
  bool m_pipette_active;    //!< true when the pipette is active and communicating, false otherwise

  bool m_simulationOnly;    //!< if active the software will run without the hardware device connected
  
  // Threding
  Labonatip_macroRunner *m_macroRunner_thread;
  QTimer *m_update_flowing_sliders;        //!< connected to an update visualization function relative to solutions flow
  int m_flowing_solution;            //!< needed for the visualization function relative to solution 1 - 2 - 3- 4
  QTimer *m_update_GUI;            //!< update GUI to show PPC1 values
  const int m_base_time_step;         //!< used to set the update timers, every step is by default 1000 ms

  //!< set the multiplicators for the time step, 
  //    e.g. desired_duration (sec) = multiplicator * m_base_time_step (100ms)
  //    TODO: KNOWN ISSUE: timing is not well fulfilled --- it is longer than expected
  int m_time_multipilcator;   //!< used to set the update time for the timers
  int m_timer_solution;               //!< duration of injection for solution 
  
  //GUI stuff for drawing solution flow
  QPen m_pen_line;                    //!< pen to draw the solution inside the pipe
  const int m_pen_line_width;         //!< pen line width, value = 5
  const float l_x1;                   //x-coordinate of the line starting point, value = -18.0
  const float l_y1;                   //y-coordinate of the line starting point, value = 49.0  
  const float l_x2;                   //x-coordinate of the line ending point, value = 55.0  
  const float l_y2;                   //y-coordinate of the line ending point, value = l_y1

  QColor m_sol1_color;      //!< my solution 1 color
  QColor m_sol2_color;      //!< my solution 2 color
  QColor m_sol3_color;      //!< my solution 3 color
  QColor m_sol4_color;      //!< my solution 4 color

  // objects for the chart
  QtCharts::QChartView *m_chartView;
  Labonatip_chart *m_labonatip_chart_view;

  double default_pon;
  double default_poff;
  double default_v_recirc;
  double default_v_switch;
  double m_pon_set_point;
  double m_poff_set_point;
  double m_v_recirc_set_point;
  double m_v_switch_set_point;

  double m_ds_perc;  //!< droplet size percentage
  double m_fs_perc;  //!< flow speed percentage
  double m_v_perc;   //!< vacuum percentage

  QString m_version;
  QString m_macro_path;
  QString m_settings_path;
  QString m_ext_data_path;
  QTranslator m_translator;

  QPixmap * led_green;
  QPixmap * led_red;
  QPainter * painter_led_green;
  QPainter * painter_led_red;

  QGroupBox * m_g_spacer;
  QAction *m_a_spacer;
};

#endif /* Labonatip_GUI_H_ */
