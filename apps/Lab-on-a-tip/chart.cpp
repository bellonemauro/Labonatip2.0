/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "chart.h"
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAbstractAxis>


Labonatip_chart::Labonatip_chart(  ) 
{
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << " Labonatip_chart::Labonatip_chart initialization " << endl;

	chart_width = 10.0;
	
	min_series_pon = 80.0;
	min_series_poff = 70.0;
	min_series_V_recirc = 60.0;
	min_series_V_switch = 50.0;
	min_series_solution = 40.0;
	min_series_ask = 30.0;
	min_series_sync_in = 20.0;
	min_series_sync_out = 10.0;

	max_pon = 450;
	max_poff = 450;
	max_v_recirc = 300;
	max_v_switch = 300;

	m_base_sol_value = 42.0;
	m_top_sol_value = 48.0;

	m_pen_s1 = new QPen();
	m_pen_s2 = new QPen();
	m_pen_s3 = new QPen();
	m_pen_s4 = new QPen();
	m_col_sol1 = new QColor(0xFFBA00); //(255, 189, 0);
	m_col_sol2 = new QColor(0xFF2800);
	m_col_sol3 = new QColor(0x009EFF);
	m_col_sol4 = new QColor(0x82FF00);

	//initialize chart objects
	m_series_Pon = new QtCharts::QLineSeries();
	m_series_Poff = new QtCharts::QLineSeries();
	m_series_V_switch = new QtCharts::QLineSeries();
	m_series_V_recirc = new QtCharts::QLineSeries();
	m_series_solution1 = new QtCharts::QLineSeries();
	m_series_solution2 = new QtCharts::QLineSeries();
	m_series_solution3 = new QtCharts::QLineSeries();
	m_series_solution4 = new QtCharts::QLineSeries();
	m_area_solution1 = new QtCharts::QAreaSeries();
	m_area_solution2 = new QtCharts::QAreaSeries();
	m_area_solution3 = new QtCharts::QAreaSeries();
	m_area_solution4 = new QtCharts::QAreaSeries();
	m_series_solution = new QtCharts::QLineSeries();
	m_series_ask = new QtCharts::QLineSeries();
	m_series_sync_in = new QtCharts::QLineSeries();
	m_series_sync_out = new QtCharts::QLineSeries();
	m_time_line_b = new QtCharts::QLineSeries();
	m_time_line_t = new QtCharts::QLineSeries();

	// add two points for each series, start and end
	*m_series_Pon << QPointF(0.0, 85.0) << QPointF(100.0, 85.0);
	*m_series_Poff << QPointF(0.0, 75.0) << QPointF(100.0, 75.0);
	*m_series_V_switch << QPointF(0.0, 65.0) << QPointF(100.0, 65.0);
	*m_series_V_recirc << QPointF(0.0, 55.0) << QPointF(100.0, 55.0);
	*m_series_solution << QPointF(0.0, m_base_sol_value) << QPointF(100.0, m_base_sol_value);
	*m_series_ask << QPointF(0.0, 35.0) << QPointF(100.0, 35.0);
	*m_series_sync_in << QPointF(0.0, 25.0) << QPointF(100.0, 25.0);
	*m_series_sync_out << QPointF(0.0, 15.0) << QPointF(100.0, 15.0);


	QPen pen_as(0x333333); // pen for ask and sync series
	pen_as.setWidth(3);

	m_series_ask->setPointsVisible(true);
	m_series_ask->setPen(pen_as);

	m_series_sync_in->setPointsVisible(true);
	m_series_sync_in->setPen(pen_as);

	m_series_sync_out->setPointsVisible(true);
	m_series_sync_out->setPen(pen_as);

	// set the vertical time line
	*m_time_line_b << QPointF(0.0, 0.0) << QPointF(0.0, 100.0);
	*m_time_line_t << QPointF(1.0, 0.0) << QPointF(1.0, 100.0);

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
	labelsFont.setPixelSize(12);
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
	axisY->append("Sync Out", min_series_sync_out + chart_width);
	axisY->append("Sync In", min_series_sync_in + chart_width);
	axisY->append("Ask", min_series_ask + chart_width);
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

	m_chart->setAxisY(axisY, m_series_Pon);
	m_chart->setAxisY(axisY, m_series_Poff);
	m_chart->setAxisY(axisY, m_series_V_switch);
	m_chart->setAxisY(axisY, m_series_V_recirc);
	m_chart->setAxisY(axisY, m_area_solution1);
	m_chart->setAxisY(axisY, m_area_solution2);
	m_chart->setAxisY(axisY, m_area_solution3);
	m_chart->setAxisY(axisY, m_area_solution4);
	m_chart->setAxisY(axisY, m_series_ask);
	m_chart->setAxisY(axisY, m_series_sync_in);
	m_chart->setAxisY(axisY, m_series_sync_out);
	m_chart->setAxisX(axisX, m_past_time_area);

	m_chart->setMargins(QMargins(0, 0, 8, 0));
	m_chart->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chartView = new QtCharts::QChartView(m_chart);
	m_chartView->setRenderHint(QPainter::Antialiasing);
	m_chartView->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));
}

void Labonatip_chart::updateChartTime(int _time_value)
{
	m_time_line_b->clear();
	m_time_line_t->clear();
	*m_time_line_b << QPointF(_time_value, 0.0) << QPointF(_time_value, 100.0);
	*m_time_line_t << QPointF(_time_value + 1.0, 0.0) << QPointF(_time_value + 1.0, 100.0);

	m_chart->update();
}


void Labonatip_chart::updateChartProtocol(f_protocol *_protocol)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_chart::updateChartMacro   " << endl;
	
	
	//TODO:: check if _macro is a valid pointer
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
	m_series_solution->clear();
	m_series_sync_in->clear();
	m_series_sync_out->clear();

	double current_time = 0.0; //!> starts from zero and will be updated according to the duration of the macro
	double max_time_line = 100.0;  //!> the duration is scaled in the interval [0; 100]
	

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
		min_series_ask + 5.0);  // + 5.0 just to set the line in the middle for now
	m_series_sync_in->append(current_time,
		min_series_sync_in + 5.0); // + 5.0 just to set the line in the middle for now
	m_series_sync_out->append(current_time,
		min_series_sync_out + 5.0); // + 5.0 just to set the line in the middle for now
	// this for now is just to show a straight line

	m_series_solution->append(max_time_line,
		m_base_sol_value ); 
	m_series_ask->append(max_time_line,
		min_series_ask + 5.0);  // + 5.0 just to set the line in the middle for now
	m_series_sync_in->append(max_time_line, 
		min_series_sync_in + 5.0); // + 5.0 just to set the line in the middle for now
	m_series_sync_out->append(100.0, 
		min_series_sync_out + 5.0); // + 5.0 just to set the line in the middle for now


	// if the macro is empty it does not update the chart
	m_chart->update();
	if (_protocol->size() < 1) return;  //TODO: this was commented, why ?

	// compute the duration of the macro
	double total_duration = 0.0;
	for (size_t i = 0; i < _protocol->size(); i++) {
		if (_protocol->at(i).getInstruction() == fluicell::PPC1api::command::instructions::wait)
			total_duration += _protocol->at(i).getValue();
	}

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_GUI::updateChartMacro ::: the complete duration is : " << total_duration << endl;

	
	for (size_t i = 0; i < _protocol->size(); i++) {
		// in every iteration a new segment is added to the chart
		// hence two points are always needed

		switch (_protocol->at(i).getInstruction())
		{
		case 0: { // Pon
			// remove the tail of the chart
			if (m_series_Pon->count()>1) m_series_Pon->remove(m_series_Pon->at(m_series_Pon->count()-1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_Pon->at(m_series_Pon->count() - 1).y(); // last added point 
			double second_x = current_time;
			double normalization_pon = max_pon / chart_width; // the values are normalized in the height reserved for the chart
			double second_y = min_series_pon + _protocol->at(i).getValue() / normalization_pon;  // new point

			m_series_Pon->append(first_x, first_y); // add the fist point
			m_series_Pon->append(second_x, second_y); // add the second point 

			//the last point is added at each step, and it must be removed every time a new point is added
			m_series_Pon->append(max_time_line, second_y);  

			break;
		}
		case 1: { // Poff
			// remove the tail of the chart
			if (m_series_Poff->count()>1) m_series_Poff->remove(m_series_Poff->at(m_series_Poff->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_Poff->at(m_series_Poff->count() - 1).y(); // last added point 
			double second_x = current_time;
			double normalization_poff = max_poff / chart_width; // the values are normalized in the height reserved for the chart
			double second_y = min_series_poff + _protocol->at(i).getValue() / normalization_poff;  // new point

			m_series_Poff->append(first_x, first_y); // add the fist point
			m_series_Poff->append(second_x, second_y); // add the second point 

			//the last point is added at each step, and it must be removed every time a new point is added
			m_series_Poff->append(max_time_line, second_y);

			break;
		}
		case 2: { // v_switch
			// remove the tail of the chart
			if (m_series_V_switch->count()>1)  m_series_V_switch->remove(m_series_V_switch->at(m_series_V_switch->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_V_switch->at(m_series_V_switch->count() - 1).y(); // last added point 
			double second_x = current_time;
			double normalization_vs = max_v_switch / chart_width; // the values are normalized in the height reserved for the chart
			double second_y = min_series_V_switch - _protocol->at(i).getValue() / normalization_vs;  // new point

			m_series_V_switch->append(first_x, first_y); // add the fist point
			m_series_V_switch->append(second_x, second_y); // add the second point 

													   //the last point is added at each step, and it must be removed every time a new point is added
			m_series_V_switch->append(max_time_line, second_y);
			break;
		}
		case 3: { // V_recirc
			// remove the tail of the chart
			if (m_series_V_recirc->count()>1) m_series_V_recirc->remove(m_series_V_recirc->at(m_series_V_recirc->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_V_recirc->at(m_series_V_recirc->count() - 1).y(); // last added point 
			double second_x = current_time;
			double normalization_vr = max_v_recirc / chart_width; // the values are normalized in the height reserved for the chart
			double second_y = min_series_V_recirc - _protocol->at(i).getValue() / normalization_vr;  // new point

			m_series_V_recirc->append(first_x, first_y); // add the fist point
			m_series_V_recirc->append(second_x, second_y); // add the second point 

			//the last point is added at each step, and it must be removed every time a new point is added
			m_series_V_recirc->append(max_time_line, second_y);
			break;
		}
		case 4: { //solution 1
				  // the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double second_x = current_time;
			double first_y = 0;
			double second_y = 0;
			if (_protocol->at(i).getValue() == 1) {
				first_y = m_base_sol_value;
 				second_y = m_top_sol_value;  // new point

			}
			else {
				first_y = m_top_sol_value;
				second_y = m_base_sol_value;
			}

			m_series_solution1->append(first_x, first_y); // add the fist point
			m_series_solution1->append(second_x, second_y); // add the second point 

			break;
		}
		case 5: { //solution 2
			 // the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double second_x = current_time;
			double first_y = 0;
			double second_y = 0;
			if (_protocol->at(i).getValue() == 1) {
				first_y = m_base_sol_value;
				second_y = m_top_sol_value;  // new point

			}
			else {
				first_y = m_top_sol_value;
				second_y = m_base_sol_value;
			}

			m_series_solution2->append(first_x, first_y); // add the fist point
			m_series_solution2->append(second_x, second_y); // add the second point 

			break;
		}
		case 6: { //solution 3
			double first_x = current_time;
			double second_x = current_time;
			double first_y = 0;
			double second_y = 0;
			if (_protocol->at(i).getValue() == 1) {
				first_y = m_base_sol_value;
				second_y = m_top_sol_value;  // new point

			}
			else {
				first_y = m_top_sol_value;
				second_y = m_base_sol_value;
			}

			m_series_solution3->append(first_x, first_y); // add the fist point
			m_series_solution3->append(second_x, second_y); // add the second point 
			break;
		}
		case 7: { //solution 4
			double first_x = current_time;
			double second_x = current_time;
			double first_y = 0;
			double second_y = 0;
			if (_protocol->at(i).getValue() == 1) {
				first_y = m_base_sol_value;
				second_y = m_top_sol_value;  // new point

			}
			else {
				first_y = m_top_sol_value;
				second_y = m_base_sol_value;
			}

			m_series_solution4->append(first_x, first_y); // add the fist point
			m_series_solution4->append(second_x, second_y); // add the second point 
			break;
		}	
		case 8: { //sleep ---- update the current time
			current_time +=  100.0 * _protocol->at(i).getValue() / total_duration; //the duration is scaled in the interval [0; 100]
			break;
		}
		case 9: { //ask_msg
			m_series_ask->append(current_time, min_series_ask + 5.0);
			break;
		}
		case 10: { //allOff
			break;
		}
		case 11: { //pumpsOff
			break;
		}
		case 12: { //waitSync
			m_series_sync_in->append(current_time, min_series_sync_in + 5.0);
			break;
		}
		case 13: { //syncOut
			m_series_sync_out->append(current_time, min_series_sync_out + 5.0);
			break;
		}
		case 14: { //dropletSize 
			break;
		}
		case 15: { //flowSpeed
			break;
		}
		case 16: { //vacuum
			break;
		}
		case 17: { //loop
			break;
		}
		default:
			break;
		}

	}
	m_chart->update();
}


Labonatip_chart::~Labonatip_chart()
{
	delete m_series_Pon;
	delete m_series_Poff;
	delete m_series_V_switch;
	delete m_series_V_recirc;
	delete m_series_solution1;
	delete m_series_solution2;
	delete m_series_solution3;
	delete m_series_solution4;
	delete m_area_solution1;
	delete m_area_solution2;
	delete m_area_solution3;
	delete m_area_solution4;
	delete m_pen_s1;
	delete m_pen_s2;
	delete m_pen_s3;
	delete m_pen_s4;
	delete m_col_sol1;
	delete m_col_sol2;
	delete m_col_sol3;
	delete m_col_sol4;
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