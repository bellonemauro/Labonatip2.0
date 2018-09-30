/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_chart_H_
#define Labonatip_chart_H_

// Qt
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>

// PPC1api test
#include <fluicell/ppc1api/ppc1api.h>

class protocolChart 
{

	typedef std::vector<fluicell::PPC1api::command> f_protocol; // define a type for fluicel protocol

public:

	explicit protocolChart( );  //!>  Ctor
	
	~protocolChart();           //!> Dtor

	/** \brief Get the chart view 
	*
	*  The chart is created in this class and attached 
	*  using a QChartView in the main GUI, this function
	*  allow the GUI to access the constantly updated chart
	*
	*  \return: QtCharts::QChartView the chart view
	*/
	QtCharts::QChartView* getChartView() {
		return m_chartView;
	}

	/** \brief Set solution 1 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor1(QColor _col_sol) {
		*m_col_sol1 = _col_sol; 
		m_pen_s1->setColor(*m_col_sol1);
		m_area_solution1->setPen(*m_pen_s1);
		m_area_solution1->setColor(*m_col_sol1);
	
	}

	/** \brief Set solution 2 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor2(QColor _col_sol) {
		*m_col_sol2 = _col_sol;
		m_pen_s2->setColor(*m_col_sol2);
		m_area_solution2->setPen(*m_pen_s2);
		m_area_solution2->setColor(*m_col_sol2);
		}

	/** \brief Set solution 3 color
	*
	*  This set of color allow the dynamic change
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor3(QColor _col_sol) { 
		*m_col_sol3 = _col_sol;
		m_pen_s3->setColor(*m_col_sol3); 
		m_area_solution3->setPen(*m_pen_s3);
		m_area_solution3->setColor(*m_col_sol3);
	}

	/** \brief Set solution 4 color
	*
	*  This set of color allow the dynamic change 
	*  of colors in the chart
	*
	*  \note: changes private data members
	*/
	void setSolutionColor4(QColor _col_sol) { 
		*m_col_sol4 = _col_sol; 
		m_pen_s4->setColor(*m_col_sol4); 
		m_area_solution4->setPen(*m_pen_s4);
		m_area_solution4->setColor(*m_col_sol4);
	}

	/** \brief Update the chart time line during the execution of a protocol
	*
	*/
	void updateChartTime(int _time_value);

	/** \brief Update the chart when a protocol is loaded
	*
	*   Interpret all the values in a protocol, including the 
	*   time line, to build an interactive chart based on the loaded protocol
	*
	*/
	void updateChartProtocol(f_protocol *_protocol);


private:
	
	/** \brief Append a new point to Pon serie
	*
	*   Append a new point to Pon serie
	*
	*
	*  \note: changes private data member m_series_pon
	*/
	void appendPonPoint(double _current_time, double _value);

	/** \brief Append a new point to Pof serie
	*
	*   Append a new point to Poff serie
	*
	*
	*  \note: changes private data member m_series_poff
	*/
	void appendPoffPoint(double _current_time, double _value);

	/** \brief Append a new point to Vrecirc serie
	*
	*   Append a new point to Vrecirc serie
	*
	*
	*  \note: changes private data member 
	*/
	void appendVrPoint(double _current_time, double _value);

	/** \brief Append a new point to Vswitch serie
	*
	*   Append a new point to Vswitch serie
	*
	*
	*  \note: changes private data member
	*/
	void appendVsPoint(double _current_time, double _value);

	/** \brief Append a new point to solutions serie
	*
	*   Append a new point to solution series
	*
	*/
	void appendSolutionPoint(QtCharts::QLineSeries *_serie, double _current_time, double _value);
	
	// data members to build the chart, 
	// in this case private pointers to the data are preferred to have simple updates
	// the chart is made using series of points for each line and areas for solutions
	QtCharts::QLineSeries *m_series_Pon;         //!> Pon series 
	QtCharts::QLineSeries *m_series_Poff;        //!> Poff series 
	QtCharts::QLineSeries *m_series_V_switch;    //!> Vswitch series 
	QtCharts::QLineSeries *m_series_V_recirc;    //!> Vrecirc series 
	QtCharts::QLineSeries *m_series_solution1;   //!> Solution 1 series 
	QtCharts::QLineSeries *m_series_solution2;   //!> Solution 2 series 
	QtCharts::QLineSeries *m_series_solution3;   //!> Solution 3 series 
	QtCharts::QLineSeries *m_series_solution4;   //!> Solution 4 series 
	QtCharts::QAreaSeries *m_area_solution1;     //!> Solution 1 area 
	QtCharts::QAreaSeries *m_area_solution2;     //!> Solution 2 area 
	QtCharts::QAreaSeries *m_area_solution3;     //!> Solution 3 area 
	QtCharts::QAreaSeries *m_area_solution4;     //!> Solution 4 area 
	QPen *m_pen_s1;        //!> Pen for solution 1, used to allow dynamic color change 
	QPen *m_pen_s2;        //!> Pen for solution 2, used to allow dynamic color change
	QPen *m_pen_s3;        //!> Pen for solution 3, used to allow dynamic color change
	QPen *m_pen_s4;        //!> Pen for solution 4, used to allow dynamic color change
	QColor *m_col_sol1;    //!> Color for solution 1, used to allow dynamic color change
	QColor *m_col_sol2;    //!> Color for solution 2, used to allow dynamic color change
	QColor *m_col_sol3;    //!> Color for solution 3, used to allow dynamic color change
	QColor *m_col_sol4;    //!> Color for solution 4, used to allow dynamic color change
	QtCharts::QLineSeries *m_series_solution;
	QtCharts::QLineSeries *m_series_ask;           //!> Ask series
	QtCharts::QLineSeries *m_series_sync_in;       //!> SyincIn series
	QtCharts::QLineSeries *m_series_sync_out;      //!> SyincOut series
	QtCharts::QLineSeries *m_time_line_b;          //!> Time line seies (base)
	QtCharts::QLineSeries *m_time_line_t;          //!> Time line seies (top)
	QtCharts::QAreaSeries *m_past_time_area;       //!> The time line is made using an area between top and base
	QtCharts::QChart *m_chart;                     //!> The chart
	QtCharts::QChartView *m_chartView;             //!> The chart view

	// this variables configure the chart in the advanced panel  
	// to show all the data lines in different levels
	double chart_width;          //!> this is the height of section in the chart, 10 units reserve per line, = 10.0;
	double min_series_pon;       //!> the Pon series will start from this level, = 80.0;
	double min_series_poff;      //!> the Poff series will start from this level, = 70.0;
	double min_series_V_recirc;  //!> the Vrecirc series will start from this level, = 60.0;
	double min_series_V_switch;  //!> the Vswitch series will start from this level, = 50.0;
	double min_series_solution;  //!> the Solution series will start from this level, = 40.0;
	double min_series_ask;       //!> the Ask series will start from this level, = 30.0;
	double min_series_sync_in;   //!> the SyncIn series will start from this level, = 20.0;
	double min_series_sync_out;  //!> the SyncOut series will start from this level, = 10.0;
	
	double max_pon;// = 450;
	double max_poff;// = 450;
	double max_v_recirc;// = 300;
	double max_v_switch;// = 300;
	double max_time_line;// = 100;
	double m_base_sol_value;    //!> Base value for the solution area drawing
	double m_top_sol_value;     //!> Top value for the solution area drawing
};


#endif /* Labonatip_chart_H_ */
