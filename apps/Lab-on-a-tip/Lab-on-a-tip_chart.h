/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_chart_H_
#define Labonatip_chart_H_

// standard libraries
#include <iostream>
#include <string>

#include "Lab-on-a-tip.h"


// PPC1api test
#include <fluicell/ppc1api/ppc1api.h>

// QT threads
#include <qthread.h>
#include <qmutex.h>

using namespace std;



class Labonatip_chart 
{
//	Q_OBJECT
	typedef std::vector<fluicell::PPC1api::command> f_macro; // define a type for fluicel macro
public:

	explicit Labonatip_chart( );
	
	~Labonatip_chart() {};

	QtCharts::QChartView* getChartView() {
		return m_chartView;
	}

	void setGUIchart();

	void updateChartTime(int _time_value);

	void updateChartMacro(f_macro *_macro);


public: //private:

	//std::vector<fluicell::PPC1api::command> *m_macro;

	// line series for the chart
	QtCharts::QLineSeries *m_series_X;
	QtCharts::QLineSeries *m_series_Y;
	QtCharts::QLineSeries *m_series_Pon;
	QtCharts::QLineSeries *m_series_Poff;
	QtCharts::QLineSeries *m_series_V_switch;
	QtCharts::QLineSeries *m_series_V_recirc;
	QtCharts::QLineSeries *m_series_solution1;
	QtCharts::QLineSeries *m_series_solution2;
	QtCharts::QLineSeries *m_series_solution3;
	QtCharts::QLineSeries *m_series_solution4;
	QtCharts::QAreaSeries *m_area_solution4;
	QtCharts::QLineSeries *m_series_solution;
	QtCharts::QLineSeries *m_series_ask;
	QtCharts::QLineSeries *m_series_sync_in;
	QtCharts::QLineSeries *m_series_sync_out;
	QtCharts::QLineSeries *m_time_line_b;
	QtCharts::QLineSeries *m_time_line_t;
	QtCharts::QAreaSeries *m_past_time_area;
	QtCharts::QChart *m_chart;

	QtCharts::QChartView *m_chartView;

	double chart_width;// = 10.0;  //!> this is the height of each line in the chart, we have reserved 10 units for each line
	double min_series_pon;// = 80.0;
	double min_series_poff;// = 70.0;
	double min_series_V_recirc;// = 60.0;
	double min_series_V_switch;// = 50.0;
	double min_series_solution;// = 40.0;
	double min_series_ask;// = 30.0;
	double min_series_sync_in;// = 20.0;
	double min_series_sync_out;// = 10.0;
	
	double max_pon;// = 450;
	double max_poff;// = 450;
	double max_v_recirc;// = 300;
	double max_v_switch;// = 300;
};


#endif /* Labonatip_macroRunner_H_ */
