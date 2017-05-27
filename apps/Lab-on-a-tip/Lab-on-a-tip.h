/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_GUI_H_
#define Labonatip_GUI_H_

// standard libraries
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
// Qt
#include <QApplication>
#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include "ui_Lab-on-a-tip.h"
#include "ui_Lab-on-a-tip_tools.h"

#include <QTextStream>
#include <QCoreApplication>
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

// QT serial communication
#include <QPlainTextEdit>
#include <QtSerialPort/QSerialPort>

//#include "Q_DebugStream.h"
#include "Lab-on-a-tip_tools.h"
#include "Lab-on-a-tip_macroRunner.h"

//#include <Eigen/Geometry>

// serial
#include <serial/serial.h>
// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>


using namespace std;

namespace Ui
{
  class Labonatip_GUI;
}


class Labonatip_tools;
class Labonatip_macroRunner;

// new shorter class name = Lab-on-a-tip  -- Loat 
class Labonatip_GUI : public QMainWindow
{
  Q_OBJECT


public:
  explicit Labonatip_GUI(QMainWindow *parent = nullptr);

  ~Labonatip_GUI();

  void setVersion(string _version);



private slots:

	/** \brief open a file 
	  * TODO : - this is just a support for the future, still not used
      * \note 
      */
  void openFile();

	/** \brief save a file 
	  * TODO : - this is just a support for the future, still not used
      * \note 
      */
  void saveFile();

  
	/** \brief This function shows a tool dialog, 
	  *        all the settings must be implemented here
      * 
      * \note 
      */
   void showToolsDialog( );

   
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


	/** \brief This function is called when the down arrow on Pon is called
	  *        it decreases the pressure on Pon, it does not accept out-of-range
	  *  
      * \note 
      */
   void pressurePonDown( );
   
 	/** \brief This function is called when the up arrow on Pon is called
	  *        it increases the pressure on Pon, it does not accept out-of-range 
	  *
      * \note 
      */
   void pressurePonUp( );

	 /** \brief This function is called when the down arrow on Poff is called
	   *        it decreases the pressure on Poff, it does not accept out-of-range
	   *
	   * \note
	   */
   void pressurePoffDown( );
   
   /** \brief This function is called when the up arrow on Poff is called
   *          it increases the pressure on Poff, it does not accept out-of-range
   *
   * \note
   */
   void pressurePoffUp( );

   /** \brief This function is called when the down arrow on v_switch is called
   *          it decreases the vacuum, it does not accept out-of-range
   *
   * \note 
   */
   void pressButtonPressed_switchDown( );
  
   /** \brief This function is called when the up arrow on v_switch is called
   *          it increases the vacuum, it does not accept out-of-range
   *
   * \note
   */
   void pressButtonPressed_switchUp( );  


   /** \brief This function is called when the down arrow on v_recirc is called
   *          it decreases the vacuum, it does not accept out-of-range
   *
   * \note
   */
   void recirculationDown( );
   
   /** \brief This function is called when the up arrow on v_recirc is called
   *          it increases the vacuum, it does not accept out-of-range
   *
   * \note
   */
   void recirculationUp( );
   

   /** \brief reboot the PPC1 
	  * 
      * \note 
      */
   void reboot( );


	/** \brief Connect and disconnect the PPC1
	  *  
      * \note 
      */
   void disCon( );

	/** \brief Run --- still work in progress
	  *  
      * \note 
      */
   void operationalMode( );

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
   
   /** \brief Update macro status message
   *  
   * \note
   */
   void updateMacroStatusMessage(const QString &_message);

   /** \brief Stop all pumps and close the valves
	  * 
      * \note 
      */
   void pumpingOff( );

	/** \brief Run the shutdown procedure
	  * 
      * \note 
      */
   void shutdown( );
   
   /** \brief  standby
   * 
   * \note
   */
   void standby();

   /** \brief Stop button should run the single command allOff()
   *  
   */
   void allOff();
   
   /** \brief Open/close the dock for advaced tools
     *
     * \note 
     */
   void closeOpenDockTools( );

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

   /** \brief Increase/reduce the area for the solution depiction
   *  
   * \note
   */
   void sliderPoffChanged(int _value);

   /** \brief  Increase/reduce the area for the solution depiction
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
   void updateTimingSlider_s1();

   void updateTimingSlider_s2();

   void updateTimingSlider_s3();
   
   void updateTimingSlider_s4();

   void updateGUI();

	/** \brief  Set the sLog file to be saved true = save // false = not save
	  *
	  *  \param  _Log_file_name   = file name 
	  *  /note - 
	  **/
	void setLogFile ( QString &_Log_file_name) 
	{		
	Log_file_name = _Log_file_name; 
	}; 

	/** \brief save log file  
	  *  
	  * /return false for any writing error
	  **/
	bool saveLog(QString &_file_name);


	/** \brief Enter simulation mode - the PPC1 will not be used
	* \note
	*/
	void simulationOnly(); 


   /** \brief  Enter what's this mode
     * \note
     */
	void  ewst();

  /** \brief Visualize the about dialog
    * \note
    */
  void  about();

  void toolOk();



private:

	void setGUIbars();

	void setGUIchart();

	void setEnableMainWindow(bool _enable = false);

	void setEnableSolutionButtons(bool _enable = false);

  /** Increase/reduce the area for the solution depiction
  *  
  * \note
  */
  void updateDrawing(int _value);

  // group all the connects are in this function
  void initConnects();

  Ui::Labonatip_GUI *ui;    //!< the user interface

  void closeEvent(QCloseEvent *event);

  QString Log_file_name;			//!< set the file name for the joints log file, default value "log_file.dat"
  QDir m_saveFolderPath;

  Labonatip_tools * m_dialog_tools; //!< pointer to the tools dialog

  QGraphicsScene *m_scene_solution;   //!< scene to draw the solution flow
  
  QPixmap *m_pmap_okIcon;   //!< pixmaps for the warning labels
  QPixmap *m_pmap_warningIcon;   //!< pixmaps for the warning labels

  // for serial communication
  QLabel *status;

  fluicell::PPC1api *m_ppc1;  //!< object for the PPC1api connection
  std::vector<fluicell::PPC1api::command> *m_macro;
  bool m_pipette_active;    //!< true when the pipette is active and communicating, false otherwise

  bool m_simulationOnly;    //!< if active the software will run without the hardware device connected
  
  // Threding
  Labonatip_macroRunner *m_macroRunner_thread;
  QTimer *m_update_time_s1;        //!< connected to an update visualization function relative to solution 1
  QTimer *m_update_time_s2;        //!< connected to an update visualization function relative to solution 2
  QTimer *m_update_time_s3;        //!< connected to an update visualization function relative to solution 3
  QTimer *m_update_time_s4;        //!< connected to an update visualization function relative to solution 4
  QTimer *m_update_GUI;            //!< update GUI to show PPC1 values
  const int m_base_time_step;         //!< used to set the update timers, every step is by default 1000 ms

  //!< set the multiplicators for the time step, 
  //    e.g. desired_duration (sec) = multiplicator * m_base_time_step (100ms)
  //    TODO: KNOWN ISSUE: timing is not well fulfilled --- it is longer then expected
  int m_time_multipilcator;   //!< used to set the update time for the timers
  int m_timer_solution;               //!< duration of injection for solution 
  
  //GUI stuff for drawing solution flow
  QPen m_pen_flow;                    //!< Pen object, used to set the border properties of the flow draw
  QRadialGradient *m_gradient_flow;    //!< Gradient of the color of the solution flow
  const float c_x;                    //!< x-coordinate of the circle, value = 53.0
  const float c_y;                    //!< y-coordinate of the circle, value = 46.0 
  const float c_radius;               //!< initial radius of the circle, value = 10.0 
  const float g_radius;               //!< radius of the gradient, value = 70.0  
  QPen m_pen_line;                    //!< pen to draw the solution inside the pipe
  const int m_pen_line_width;         //!< pen line width, value = 5
  const float l_x1;                   //x-coordinate of the line starting point, value = -18.0
  const float l_y1;                   //y-coordinate of the line starting point, value = 49.0  
  const float l_x2;                   //x-coordinate of the line ending point, value = 55.0  
  const float l_y2;                   //y-coordinate of the line ending point, value = l_y1

  // set all the GUI colors for solutions and gradients
  // NOTE: colors are not supposed to be set manually
  const QColor m_sol1_color;      //!< my solution 1 color
  const QColor m_sol1_color_g05;  //!< my solution 1 gradient color - first level
  const QColor m_sol1_color_g1;   //!< my solution 1 gradient color - second level
  
  const QColor m_sol2_color;      //!< my solution 2 color
  const QColor m_sol2_color_g05;  //!< my solution 2 gradient color - first level
  const QColor m_sol2_color_g1;   //!< my solution 2 gradient color - second level

  const QColor m_sol3_color;      //!< my solution 3 color
  const QColor m_sol3_color_g05;  //!< my solution 3 gradient color - first level
  const QColor m_sol3_color_g1;   //!< my solution 3 gradient color - second level

  const QColor m_sol4_color;      //!< my solution 4 color
  const QColor m_sol4_color_g05;  //!< my solution 4 gradient color - first level
  const QColor m_sol4_color_g1;   //!< my solution 4 gradient color - second level

  // line series for the chart
  QtCharts::QLineSeries *m_series_X;
  QtCharts::QLineSeries *m_series_Y;
  QtCharts::QLineSeries *m_series_Pon;
  QtCharts::QLineSeries *m_series_Poff;
  QtCharts::QLineSeries *m_series_V_switch;
  QtCharts::QLineSeries *m_series_V_recirc;
  QtCharts::QLineSeries *m_series_solution;
  QtCharts::QLineSeries *m_series_ask;
  QtCharts::QLineSeries *m_series_sync_in;
  QtCharts::QLineSeries *m_series_sync_out;

  QString m_version;

};

#endif /* Labonatip_GUI_H_ */
