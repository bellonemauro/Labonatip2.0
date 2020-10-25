/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "chart.h"
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAbstractAxis>
#include "dataStructures.h"


protocolChart::protocolChart(  ):
	chart_width(10.0),
	min_series_pon (80.0),
	min_series_poff (70.0),
	min_series_V_recirc(60.0),
	min_series_V_switch (50.0),
	min_series_solution (40.0),
	min_series_ask (35.0),
	min_series_sync_in (25.0),
	min_series_sync_out (15.0),
	max_pon (450),
	max_poff (450),
	max_v_recirc (300),
	max_v_switch  (300),
	max_time_line (100.0),  //!> the duration is scaled in the interval [0; 100]
	min_time_line  (0.0),  //!> the duration is scaled in the interval [0; 100]
	m_base_sol_value (42.0),
	m_top_sol_value (48.0),
	m_time_line_thickness(1.0)
{
	std::cout << HERE << std::endl;

	m_pen_s1 = new QPen();
	m_pen_s2 = new QPen();
	m_pen_s3 = new QPen();
	m_pen_s4 = new QPen();
	m_pen_s5 = new QPen();
	m_pen_s6 = new QPen();
	m_col_sol1 = new QColor(0xFFBA00); //(255, 189, 0);
	m_col_sol2 = new QColor(0xFF2800);
	m_col_sol3 = new QColor(0x009EFF);
	m_col_sol4 = new QColor(0x82FF00);
	m_col_sol5 = new QColor(0x82FF00);
	m_col_sol6 = new QColor(0x82FF00);

	//initialize chart objects
	m_series_Pon = new QtCharts::QLineSeries();
	m_series_Poff = new QtCharts::QLineSeries();
	m_series_V_switch = new QtCharts::QLineSeries();
	m_series_V_recirc = new QtCharts::QLineSeries();
	m_series_solution1 = new QtCharts::QLineSeries();
	m_series_solution2 = new QtCharts::QLineSeries();
	m_series_solution3 = new QtCharts::QLineSeries();
	m_series_solution4 = new QtCharts::QLineSeries();
	m_series_solution5 = new QtCharts::QLineSeries();
	m_series_solution6 = new QtCharts::QLineSeries();
	m_area_solution1 = new QtCharts::QAreaSeries();
	m_area_solution2 = new QtCharts::QAreaSeries();
	m_area_solution3 = new QtCharts::QAreaSeries();
	m_area_solution4 = new QtCharts::QAreaSeries();
	m_area_solution5 = new QtCharts::QAreaSeries();
	m_area_solution6 = new QtCharts::QAreaSeries();
	m_series_solution = new QtCharts::QLineSeries();
	m_series_ask = new QtCharts::QLineSeries();
	m_series_sync_in = new QtCharts::QLineSeries();
	m_series_sync_out = new QtCharts::QLineSeries();
	m_time_line_b = new QtCharts::QLineSeries();
	m_time_line_t = new QtCharts::QLineSeries();

	// add two points for each series, start and end
	*m_series_Pon << QPointF(min_time_line, min_series_pon) << QPointF(max_time_line, min_series_pon);
	*m_series_Poff << QPointF(min_time_line, min_series_poff) << QPointF(max_time_line, min_series_poff);
	*m_series_V_recirc << QPointF(min_time_line, min_series_V_recirc) << QPointF(max_time_line, min_series_V_recirc);
	*m_series_V_switch << QPointF(min_time_line, min_series_V_switch) << QPointF(max_time_line, min_series_V_switch);
	*m_series_solution << QPointF(min_time_line, m_base_sol_value) << QPointF(max_time_line, m_base_sol_value);
	*m_series_ask << QPointF(min_time_line, min_series_ask) << QPointF(max_time_line, min_series_ask);
	*m_series_sync_in << QPointF(min_time_line, min_series_sync_in) << QPointF(max_time_line, min_series_sync_in);
	*m_series_sync_out << QPointF(min_time_line, min_series_sync_out) << QPointF(max_time_line, min_series_sync_out);


	QPen pen_as(0x333333); // pen for ask and sync series
	pen_as.setWidth(3);

	m_series_ask->setPointsVisible(true);
	m_series_ask->setPen(pen_as);

	m_series_sync_in->setPointsVisible(true);
	m_series_sync_in->setPen(pen_as);

	m_series_sync_out->setPointsVisible(true);
	m_series_sync_out->setPen(pen_as);

	// set the vertical time line
	*m_time_line_b << QPointF(min_time_line, min_time_line) << QPointF(min_time_line, max_time_line);
	*m_time_line_t << QPointF(m_time_line_thickness, min_time_line) << QPointF(m_time_line_thickness, max_time_line);

	// the pen is creates as the area between two vertical lines
	m_past_time_area = new QtCharts::QAreaSeries(m_time_line_b, m_time_line_t);
	m_past_time_area->setName("Time line");
	QPen pen_t(0xFFFFFF);
	pen_t.setWidth(1);
	m_past_time_area->setPen(pen_t);
	m_time_line_b->setPen(pen_t);
	QPen pen_b(0xAAAAAA);
	pen_b.setWidth(1);
	m_time_line_t->setPen(pen_b);
	QLinearGradient gradient(QPointF(1, 50), QPointF(0, 50));
	QColor c1;
	c1.setRgb(0xAAAAAA);
	c1.setAlpha(150);
	gradient.setColorAt(0.0, c1);
	QColor c2;
	c2.setRgb(0xFFFFFF);
	c2.setAlpha(150);
	gradient.setColorAt(1.0, c2);
	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	m_past_time_area->setBrush(gradient);

	// set colors to lines for the different solutions series
	m_pen_s1->setWidth(0); //(20);
	m_pen_s1->setColor(*m_col_sol1);

	m_pen_s2->setWidth(0); //(20);
	m_pen_s2->setColor(*m_col_sol2);

	m_pen_s3->setWidth(0); //(20);
	m_pen_s3->setColor(*m_col_sol3);

	m_pen_s4->setWidth(0); //(20);
	m_pen_s4->setColor(*m_col_sol4);

	// the areas for all the solution are defines as the area between the base solution and series_solution_n
	m_area_solution1->setLowerSeries(m_series_solution);
	m_area_solution1->setUpperSeries(m_series_solution1);
	m_area_solution1->setPen(*m_pen_s1);
	m_area_solution1->setColor(*m_col_sol1);

	m_area_solution2->setLowerSeries(m_series_solution);
	m_area_solution2->setUpperSeries(m_series_solution2);
	m_area_solution2->setPen(*m_pen_s2);
	m_area_solution2->setColor(*m_col_sol2);

	m_area_solution3->setLowerSeries(m_series_solution);
	m_area_solution3->setUpperSeries(m_series_solution3);
	m_area_solution3->setPen(*m_pen_s3);
	m_area_solution3->setColor(*m_col_sol3);

	m_area_solution4->setLowerSeries(m_series_solution);
	m_area_solution4->setUpperSeries(m_series_solution4);
	m_area_solution4->setPen(*m_pen_s4);
	m_area_solution4->setColor(*m_col_sol4);

	m_area_solution5->setLowerSeries(m_series_solution);
	m_area_solution5->setUpperSeries(m_series_solution5);
	m_area_solution5->setPen(*m_pen_s5);
	m_area_solution5->setColor(*m_col_sol5);

	m_area_solution6->setLowerSeries(m_series_solution);
	m_area_solution6->setUpperSeries(m_series_solution6);
	m_area_solution6->setPen(*m_pen_s6);
	m_area_solution6->setColor(*m_col_sol6);
	
	// set the chart
	m_chart = new QtCharts::QChart();
	m_chart->legend()->hide();

	// add the series to the chart

	m_chart->addSeries(m_series_Pon);
	m_chart->addSeries(m_series_Poff);
	m_chart->addSeries(m_series_V_switch);
	m_chart->addSeries(m_series_V_recirc);
	m_chart->addSeries(m_area_solution1);
	m_chart->addSeries(m_area_solution2);
	m_chart->addSeries(m_area_solution3);
	m_chart->addSeries(m_area_solution4);
	m_chart->addSeries(m_area_solution5);
	m_chart->addSeries(m_area_solution6);
	m_chart->addSeries(m_series_ask);
	m_chart->addSeries(m_series_sync_in);
	m_chart->addSeries(m_series_sync_out);
	m_chart->addSeries(m_time_line_b);
	m_chart->addSeries(m_time_line_t);
	m_chart->addSeries(m_past_time_area);

	// set the axis
	QtCharts::QCategoryAxis *axisX = new QtCharts::QCategoryAxis();
	QtCharts::QCategoryAxis *axisY = new QtCharts::QCategoryAxis();

	// Customize axis label font
	QFont labelsFont;
	labelsFont.setPointSize(8);
	axisX->setLabelsFont(labelsFont);
	axisX->setLabelsPosition(QtCharts::QCategoryAxis::AxisLabelsPositionOnValue);
	axisY->setLabelsFont(labelsFont);

	// Customize axis colors
	QPen axisPen(QRgb(0xd18952));
	axisPen.setWidth(2);
	axisX->setLinePen(axisPen);
	axisY->setLinePen(axisPen);

	// Customize axis label colors
	QBrush axisBrush(Qt::black);
	axisX->setLabelsBrush(axisBrush);
	axisY->setLabelsBrush(axisBrush);

	// Customize grid lines and shades
	axisY->setGridLineVisible(false);
	axisY->setShadesPen(Qt::NoPen);
	axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
	axisY->setShadesVisible(true);

	// Set X axis labels
	axisX->append("20 %", 20);
	axisX->append("40 %", 40);
	axisX->append("60 %", 60);
	axisX->append("80 %", 80);
	axisX->append("100 %", 100);
	axisX->setRange(0, 100);


	axisX->setTitleText("Simulation time");

	//axisY->append(" ", 10);
	QString s;
	axisY->append("Sync Out", min_series_sync_out + chart_width/2.0);
	axisY->append("Sync In", min_series_sync_in + chart_width/2.0);
	axisY->append("Ask", min_series_ask + chart_width/2.0);
	axisY->append("Solution", min_series_solution + chart_width);
	s = QStringLiteral("<html><head/><body><p>V<span style=\" vertical-align:sub;\">switch</span></p></body></html>");
	axisY->append(s, min_series_V_switch + chart_width);

	s.clear();
	s = QStringLiteral("<html><head/><body><p>V<span style=\" vertical-align:sub;\">recirc</span></p></body></html>");
	axisY->append(s, min_series_V_recirc + chart_width);

	s.clear();
	s = QStringLiteral("<html><head/><body><p>P<span style=\" vertical-align:sub;\">off</span></p></body></html>");
	axisY->append(s, min_series_poff + chart_width);

	s.clear();
	s = QStringLiteral("<html><head/><body><p>P<span style=\" vertical-align:sub;\">on</span></p></body></html>");
	axisY->append(s, min_series_pon + chart_width);

	axisY->setRange(10, 90);

	m_chart->addAxis(axisY,Qt::AlignLeft);
	m_series_Pon->attachAxis(axisY);
	m_series_Poff->attachAxis(axisY);
	m_series_V_switch->attachAxis(axisY);
	m_series_V_recirc->attachAxis(axisY);
	m_area_solution1->attachAxis(axisY);
	m_area_solution2->attachAxis(axisY);
	m_area_solution3->attachAxis(axisY);
	m_area_solution4->attachAxis(axisY);
	m_area_solution5->attachAxis(axisY);
	m_area_solution6->attachAxis(axisY);
	m_series_ask->attachAxis(axisY);
	m_series_sync_in->attachAxis(axisY);
	m_series_sync_out->attachAxis(axisY);
	m_chart->addAxis(axisX, Qt::AlignBottom);
	m_past_time_area->attachAxis(axisX);


	m_chart->setMargins(QMargins(0, 0, 8, 0));
	m_chart->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chartView = new QtCharts::QChartView(m_chart);
	m_chartView->setRenderHint(QPainter::Antialiasing);
	m_chartView->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));
}

void protocolChart::updateChartTime(int _time_value)
{
	m_time_line_b->clear();
	m_time_line_t->clear();
	*m_time_line_b << QPointF(_time_value, min_time_line) 
		<< QPointF(_time_value, max_time_line);
	*m_time_line_t << QPointF(_time_value + m_time_line_thickness, min_time_line)
		<< QPointF(_time_value + m_time_line_thickness, max_time_line);

	m_chart->update();
}


void protocolChart::updateChartProtocol(f_protocol *_protocol)
{
	std::cout << HERE << std::endl;
	
	if (_protocol == NULL) return;      // if is null we do nothing

	m_series_Pon->clear();
	m_series_Poff->clear();
	m_series_V_recirc->clear();
	m_series_V_switch->clear();
	m_series_ask->clear();
	m_series_solution1->clear();
	m_series_solution2->clear();
	m_series_solution3->clear();
	m_series_solution4->clear();
	m_series_solution5->clear();
	m_series_solution6->clear();
	m_series_solution->clear();
	m_series_sync_in->clear();
	m_series_sync_out->clear();

	double current_time = 0.0; //!> starts from zero and will be updated according to the duration of the macro
	
	// append zero
	m_series_Pon->append(current_time,
		min_series_pon);  // in [80; 90]
	m_series_Poff->append(current_time,
		min_series_poff); // in [70; 80]
	m_series_V_recirc->append(current_time,
		min_series_V_recirc); // in [60; 70]
	m_series_V_switch->append(current_time,
		min_series_V_switch); // in [50; 60]
	m_series_solution->append(current_time,
		m_base_sol_value); 
	m_series_ask->append(current_time,
		min_series_ask );  // + 5.0 (chart_width/2) just to set the line in the middle for now
	m_series_sync_in->append(current_time,
		min_series_sync_in ); // + 5.0 just to set the line in the middle for now
	m_series_sync_out->append(current_time,
		min_series_sync_out ); // + 5.0 just to set the line in the middle for now
	// this for now is just to show a straight line

	m_series_solution->append(max_time_line,
		m_base_sol_value ); 
	m_series_ask->append(max_time_line,
		min_series_ask );  // + 5.0 just to set the line in the middle for now
	m_series_sync_in->append(max_time_line, 
		min_series_sync_in ); // + 5.0 just to set the line in the middle for now
	m_series_sync_out->append(100.0, 
		min_series_sync_out ); // + 5.0 just to set the line in the middle for now


	// if the macro is empty it does not update the chart
	m_chart->update();
	if (_protocol->size() < 1) return;  

	// compute the duration of the macro
	double total_duration = 0.0;
	for (size_t i = 0; i < _protocol->size(); i++) {
		if (_protocol->at(i).getInstruction() == ppc1Cmd::wait)
			total_duration += _protocol->at(i).getValue();
	}

	std::cout << HERE
		 << " the complete duration is : " << total_duration << std::endl;

	
	for (size_t i = 0; i < _protocol->size(); i++) {
		// in every iteration a new segment is added to the chart
		// hence two points are always needed

		switch (_protocol->at(i).getInstruction())
		{
		case ppc1Cmd::setPon: { // Pon
			appendPonPoint(current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::setPoff: { // Poff
			appendPoffPoint(current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::setVswitch: { // v_switch
			appendVsPoint(current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::setVrecirc: { // V_recirc
			appendVrPoint(current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::solution1: { //solution 1
			if (_protocol->at(i).getValue() == 1)
			{ // if we are opening the solution all the others will be closed
				appendSolutionPoint(m_series_solution2, current_time, 0);
				appendSolutionPoint(m_series_solution3, current_time, 0);
				appendSolutionPoint(m_series_solution4, current_time, 0);
				appendSolutionPoint(m_series_solution5, current_time, 0);
				appendSolutionPoint(m_series_solution6, current_time, 0);
			}
			appendSolutionPoint(m_series_solution1, current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::solution2: { //solution 2
			if (_protocol->at(i).getValue() == 1)
			{ // if we are opening the solution all the others will be closed
				appendSolutionPoint(m_series_solution1, current_time, 0);
				appendSolutionPoint(m_series_solution3, current_time, 0);
				appendSolutionPoint(m_series_solution4, current_time, 0);
				appendSolutionPoint(m_series_solution5, current_time, 0);
				appendSolutionPoint(m_series_solution6, current_time, 0);
			}
			appendSolutionPoint(m_series_solution2, current_time, _protocol->at(i).getValue()); 
			break;
		}
		case ppc1Cmd::solution3: { //solution 3
			if (_protocol->at(i).getValue() == 1)
			{ // if we are opening the solution all the others will be closed
				appendSolutionPoint(m_series_solution1, current_time, 0);
				appendSolutionPoint(m_series_solution2, current_time, 0);
				appendSolutionPoint(m_series_solution4, current_time, 0);
				appendSolutionPoint(m_series_solution5, current_time, 0);
				appendSolutionPoint(m_series_solution6, current_time, 0);
			}
			appendSolutionPoint(m_series_solution3, current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::solution4: { //solution 4
			if (_protocol->at(i).getValue() == 1)
			{ // if we are opening the solution all the others will be closed
				appendSolutionPoint(m_series_solution1, current_time, 0);
				appendSolutionPoint(m_series_solution2, current_time, 0);
				appendSolutionPoint(m_series_solution3, current_time, 0);
				appendSolutionPoint(m_series_solution5, current_time, 0);
				appendSolutionPoint(m_series_solution6, current_time, 0);
			}
			appendSolutionPoint(m_series_solution4, current_time, _protocol->at(i).getValue());
			break;
		}	
		case ppc1Cmd::solution5: { //solution 5
			if (_protocol->at(i).getValue() == 1)
			{ // if we are opening the solution all the others will be closed
				appendSolutionPoint(m_series_solution1, current_time, 0);
				appendSolutionPoint(m_series_solution2, current_time, 0);
				appendSolutionPoint(m_series_solution3, current_time, 0);
				appendSolutionPoint(m_series_solution4, current_time, 0);
				appendSolutionPoint(m_series_solution6, current_time, 0);
			}
			appendSolutionPoint(m_series_solution5, current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::solution6: { //solution 6
			if (_protocol->at(i).getValue() == 1)
			{ // if we are opening the solution all the others will be closed
				appendSolutionPoint(m_series_solution1, current_time, 0);
				appendSolutionPoint(m_series_solution2, current_time, 0);
				appendSolutionPoint(m_series_solution3, current_time, 0);
				appendSolutionPoint(m_series_solution4, current_time, 0);
				appendSolutionPoint(m_series_solution5, current_time, 0);
			}
			appendSolutionPoint(m_series_solution6, current_time, _protocol->at(i).getValue());
			break;
		}
		case ppc1Cmd::wait: { //sleep ---- update the current time
			current_time +=  100.0 * _protocol->at(i).getValue() / total_duration; //the duration is scaled in the interval [0; 100]
			break;
		}
		case ppc1Cmd::ask: { //ask_msg
			m_series_ask->append(current_time, min_series_ask );
			break;
		}
		case ppc1Cmd::allOff: { //allOff
			// if the command is allOff the solutions will be closed
			appendSolutionPoint(m_series_solution1, current_time, 0);
			appendSolutionPoint(m_series_solution2, current_time, 0);
			appendSolutionPoint(m_series_solution3, current_time, 0);
			appendSolutionPoint(m_series_solution4, current_time, 0);
			appendSolutionPoint(m_series_solution5, current_time, 0);
			appendSolutionPoint(m_series_solution6, current_time, 0);

			break;
		}
		case ppc1Cmd::pumpsOff: { //pumpsOff
			// if the command is allOff the solutions will be closed
			appendSolutionPoint(m_series_solution1, current_time, 0);
			appendSolutionPoint(m_series_solution2, current_time, 0);
			appendSolutionPoint(m_series_solution3, current_time, 0);
			appendSolutionPoint(m_series_solution4, current_time, 0);
			appendSolutionPoint(m_series_solution5, current_time, 0);
			appendSolutionPoint(m_series_solution6, current_time, 0);

			//also the pon-poff-vr-vs need to be updated to add a zero
			appendPonPoint(current_time, 0.0);
			appendPoffPoint(current_time, 0.0);
			appendVrPoint(current_time, 0.0);
			appendVsPoint(current_time, 0.0);

			break;
		}
		case ppc1Cmd::waitSync: { //waitSync
			m_series_sync_in->append(current_time, min_series_sync_in );
			break;
		}
		case ppc1Cmd::syncOut: { //syncOut
			m_series_sync_out->append(current_time, min_series_sync_out );
			break;
		}
		case ppc1Cmd::loop: { //loop
			break;
		}
		default:
			break;
		}

	}
	// this avoid the bad chart visualization if the user forget to close the solution
	appendSolutionPoint(m_series_solution1, max_time_line, 0);
	appendSolutionPoint(m_series_solution2, max_time_line, 0);
	appendSolutionPoint(m_series_solution3, max_time_line, 0);
	appendSolutionPoint(m_series_solution4, max_time_line, 0);
	appendSolutionPoint(m_series_solution5, max_time_line, 0);
	appendSolutionPoint(m_series_solution6, max_time_line, 0);
	// update
	m_chart->update();
}

void protocolChart::appendPonPoint( double _current_time, double _value)
{
	// remove the tail of the chart
	if (m_series_Pon->count()>1) m_series_Pon->remove(m_series_Pon->at(m_series_Pon->count() - 1));

	// the first point is calculated starting from the last value to the new value an the current time
	double first_x = _current_time;
	double first_y = m_series_Pon->at(m_series_Pon->count() - 1).y(); // last added point 
	double second_x = _current_time;
	double normalization_pon = max_pon / chart_width; // the values are normalized in the height reserved for the chart
	double second_y = min_series_pon + _value / normalization_pon;  // new point

	m_series_Pon->append(first_x, first_y); // add the fist point
	m_series_Pon->append(second_x, second_y); // add the second point 

	//the last point is added at each step, and it must be removed every time a new point is added
	m_series_Pon->append(max_time_line, second_y);
}

void protocolChart::appendPoffPoint( double _current_time, double _value)
{
	// remove the tail of the chart
	if (m_series_Poff->count()>1) m_series_Poff->remove(m_series_Poff->at(m_series_Poff->count() - 1));

	// the first point is calculated starting from the last value to the new value an the current time
	double first_x = _current_time;
	double first_y = m_series_Poff->at(m_series_Poff->count() - 1).y(); // last added point 
	double second_x = _current_time;
	double normalization_poff = max_poff / chart_width; // the values are normalized in the height reserved for the chart
	double second_y = min_series_poff + _value / normalization_poff;  // new point

	m_series_Poff->append(first_x, first_y); // add the fist point
	m_series_Poff->append(second_x, second_y); // add the second point 
											   //the last point is added at each step, and it must be removed every time a new point is added
	m_series_Poff->append(max_time_line, second_y);
}

void protocolChart::appendVrPoint(double _current_time, double _value)
{

	// remove the tail of the chart
	if (m_series_V_recirc->count()>1) m_series_V_recirc->remove(m_series_V_recirc->at(m_series_V_recirc->count() - 1));

	// the first point is calculated starting from the last value to the new value an the current time
	double first_x = _current_time;
	double first_y = m_series_V_recirc->at(m_series_V_recirc->count() - 1).y(); // last added point 
	double second_x = _current_time;
	double normalization_vr = max_v_recirc / chart_width; // the values are normalized in the height reserved for the chart
	double second_y = min_series_V_recirc - _value / normalization_vr;  // new point

	m_series_V_recirc->append(first_x, first_y); // add the fist point
	m_series_V_recirc->append(second_x, second_y); // add the second point 

												   //the last point is added at each step, and it must be removed every time a new point is added
	m_series_V_recirc->append(max_time_line, second_y);
}

void protocolChart::appendVsPoint(double _current_time, double _value)
{
	// remove the tail of the chart
	if (m_series_V_switch->count()>1)  m_series_V_switch->remove(m_series_V_switch->at(m_series_V_switch->count() - 1));

	// the first point is calculated starting from the last value to the new value an the current time
	double first_x = _current_time;
	double first_y = m_series_V_switch->at(m_series_V_switch->count() - 1).y(); // last added point 
	double second_x = _current_time;
	double normalization_vs = max_v_switch / chart_width; // the values are normalized in the height reserved for the chart
	double second_y = min_series_V_switch - _value / normalization_vs;  // new point

	// add the fist point
	m_series_V_switch->append(first_x, first_y); 
	// add the second point 
	m_series_V_switch->append(second_x, second_y); 

	//the last point is added at each step, and it must be removed every time a new point is added
	m_series_V_switch->append(max_time_line, second_y);
}

void protocolChart::appendSolutionPoint(QtCharts::QLineSeries *_serie, double _current_time, double _value)
{
	double first_x = _current_time;
	double second_x = _current_time;
	double first_y = 0;
	double second_y = 0;

	// if this is the first point, 
	// we need to check if it is 0 to avoid misleading points
	if (_serie->count() > 0)
	{
		// if we have more than one point we need to check the previous one
		// they must be different among each other to be drawn
		QPointF point = _serie->at(_serie->count()-1);
		if (point.y() == m_base_sol_value) // the last solution value was 0
		{
			if (_value == 1) {
				first_y = m_base_sol_value;
				second_y = m_top_sol_value;  // new point
			}
			else {
				first_y = m_base_sol_value;
				second_y = m_base_sol_value;
			}
		}
		else // the last solution value was 1
		{
			if (_value == 1) {
				first_y = m_top_sol_value;
				second_y = m_top_sol_value;  // new point
			}
			else {
				first_y = m_top_sol_value;
				second_y = m_base_sol_value;
			}
		}
	}
	else
	{ // here we assume that if we have no points the solution was OFF
	  // avoid misleading drawing
		if (_value == 1) { //turning solution ON
			// if the value is 1 we draw a point
			first_y = m_base_sol_value;
			second_y = m_top_sol_value;  // new point
		}
		else { //turning solution OFF
			// otherwise the first point does not need to be drawn
			first_y = m_base_sol_value;
			second_y = m_base_sol_value;

			return;
		}
	}

	// add the fist point
	_serie->append(first_x, first_y); 
	// add the second point 
	_serie->append(second_x, second_y); 
}


protocolChart::~protocolChart()
{
	delete m_series_Pon;
	delete m_series_Poff;
	delete m_series_V_switch;
	delete m_series_V_recirc;
	delete m_series_solution1;
	delete m_series_solution2;
	delete m_series_solution3;
	delete m_series_solution4;
	delete m_series_solution5;
	delete m_series_solution6;
	delete m_area_solution1;
	delete m_area_solution2;
	delete m_area_solution3;
	delete m_area_solution4;
	delete m_area_solution5;
	delete m_area_solution6;
	delete m_pen_s1;
	delete m_pen_s2;
	delete m_pen_s3;
	delete m_pen_s4;
	delete m_pen_s5;
	delete m_pen_s6;
	delete m_col_sol1;
	delete m_col_sol2;
	delete m_col_sol3;
	delete m_col_sol4;
	delete m_col_sol5;
	delete m_col_sol6;
	delete m_series_solution;
	delete m_series_ask;
	delete m_series_sync_in;
	delete m_series_sync_out;
	delete m_time_line_b;
	delete m_time_line_t;
	delete m_past_time_area;
	delete m_chart;

	delete m_chartView; 
}
