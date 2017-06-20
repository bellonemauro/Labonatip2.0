/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip_chart.h"
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAbstractAxis>


Labonatip_chart::Labonatip_chart(  ) 
{
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< " Labonatip_chart::Labonatip_chart initialization " << endl;
	setGUIchart();
}

void Labonatip_chart::updateChartTime(int _time_value)
{
	m_time_line_b->clear();
	m_time_line_t->clear();
	*m_time_line_b << QPointF(_time_value, 0.0) << QPointF(_time_value, 100.0);
	*m_time_line_t << QPointF(_time_value + 1.0, 0.0) << QPointF(_time_value + 1.0, 100.0);

	m_chart->update();
}


void Labonatip_chart::updateChartMacro(f_macro *_macro)
{
	//TODO:: check if _macro is a valid pointer


	m_series_Pon->clear();
	m_series_Poff->clear();
	m_series_V_recirc->clear();
	m_series_V_switch->clear();
	m_series_ask->clear();
	m_series_solution->clear();
	m_series_sync_in->clear();
	m_series_sync_out->clear();

	// append zero
	m_series_Pon->append(0.0, 80.0);  // in [80; 90]
	m_series_Poff->append(0.0, 70.0); // in [70; 80]
	m_series_V_recirc->append(0.0, 60.0); // in [60; 70]
	m_series_V_switch->append(0.0, 50.0); // in [50; 60]
	m_series_ask->append(0.0, 45.0);
	m_series_solution->append(0.0, 35.0);
	m_series_sync_in->append(0.0, 25.0);
	m_series_sync_out->append(0.0, 15.0);
	// this for now is just to show a straight line

	m_series_ask->append(100.0, 45.0);
	m_series_solution->append(100.0, 35.0);
	m_series_sync_in->append(100.0, 25.0);
	m_series_sync_out->append(100.0, 15.0);

	// if the macro is empty it does not update the chart
	//if (m_macro->size() < 1) return;

	// compute the duration of the macro
	double duration = 0.0;
	for (int i = 0; i < _macro->size(); i++)
		duration += _macro->at(i).Duration;

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::updateChartMacro ::: the complete duration is : " << duration << endl;


	for (int i = 0; i < _macro->size(); i++) {
		// TODO CONTINUE FROM HERE !!!! 
		//double next_x = m_series_Pon->at(m_series_Pon->count() - 1).x();
		//double next_y = 80 + m_macro->at(i).P_on / 50;
		//double next_next_x = m_series_Pon->at(m_series_Pon->count() - 1).x();
		//double duration_x = 100.0 * double(m_macro->at(i).Duration)/ duration;

		//cout << " next x is " << next_x 
		//	<< " next y is " << next_y 
		//	<< " next next x is " << next_next_x 
		//	<< " the new duration is " << duration_x << endl;
		m_series_Pon->append(//next_x, next_y);
			m_series_Pon->at(m_series_Pon->count() - 1).x(),
			80 + _macro->at(i).P_on / 50);  // in [80; 90]

		m_series_Pon->append(
			m_series_Pon->at(m_series_Pon->count() - 1).x() + 100.0 * double(_macro->at(i).Duration) / duration, //qreal(m_macro->at(i).Duration),
			80 + _macro->at(i).P_on / 50);  // in [80; 90]

		m_series_Poff->append(
			m_series_Poff->at(m_series_Poff->count() - 1).x(),
			70 + _macro->at(i).P_off / 50);// m_macro->at(i).P_off); // in [70; 80]
		m_series_Poff->append(
			m_series_Poff->at(m_series_Poff->count() - 1).x() + 100.0 * double(_macro->at(i).Duration) / duration,
			70 + _macro->at(i).P_off / 50);// m_macro->at(i).P_off); // in [70; 80]

		m_series_V_recirc->append(
			m_series_V_recirc->at(m_series_V_recirc->count() - 1).x(),
			60 - _macro->at(i).V_recirc / 35);// m_macro->at(i).P_off); // in [70; 80]
		m_series_V_recirc->append(
			m_series_V_recirc->at(m_series_V_recirc->count() - 1).x() + 100.0 * double(_macro->at(i).Duration) / duration,
			60 - _macro->at(i).V_recirc / 35);// m_macro->at(i).P_off); // in [70; 80]

		m_series_V_switch->append(
			m_series_V_switch->at(m_series_V_switch->count() - 1).x(),
			50 - _macro->at(i).V_switch / 35);// m_macro->at(i).P_off); // in [70; 80]
		m_series_V_switch->append(
			m_series_V_switch->at(m_series_V_switch->count() - 1).x() + 100.0 * double(_macro->at(i).Duration) / duration,
			50 - _macro->at(i).V_switch / 35);// m_macro->at(i).P_off); // in [70; 80]
											   //		m_series_V_recirc->append(
											   //			m_series_Pon->at(m_series_Pon->count() - 1).x() + qreal(m_macro->at(i).Duration / duration), 
											   //			i * 12);//m_macro->at(i).V_recirc); // in [60; 70]
											   //		m_series_V_switch->append(
											   //			m_series_Pon->at(m_series_Pon->count() - 1).x() + qreal(m_macro->at(i).Duration / duration), 
											   //			i * 15);//m_macro->at(i).V_switch); // in [50; 60]

	}



	m_chart->update();
}

void Labonatip_chart::setGUIchart()
{

	//![1]
	m_series_X = new QtCharts::QLineSeries();
	m_series_Y = new QtCharts::QLineSeries();
	m_series_Pon = new QtCharts::QLineSeries();
	m_series_Poff = new QtCharts::QLineSeries();
	m_series_V_switch = new QtCharts::QLineSeries();
	m_series_V_recirc = new QtCharts::QLineSeries();
	m_series_solution = new QtCharts::QLineSeries();
	m_series_ask = new QtCharts::QLineSeries();
	m_series_sync_in = new QtCharts::QLineSeries();
	m_series_sync_out = new QtCharts::QLineSeries();
	m_time_line_b = new QtCharts::QLineSeries();
	m_time_line_t = new QtCharts::QLineSeries();


	//![1]

	//![2]
	*m_series_X << QPointF(0.0, 0.0) << QPointF(10.0, 0.0)
		<< QPointF(20.0, 0.0) << QPointF(30.0, 0.0)
		<< QPointF(40.0, 0.0) << QPointF(50.0, 0.0)
		<< QPointF(60.0, 0.0) << QPointF(70.0, 0.0)
		<< QPointF(80.0, 0.0) << QPointF(90.0, 0.0)
		<< QPointF(100.0, 0.0);

	*m_series_Y << QPointF(0.0, 0.0) << QPointF(0.0, 10.0)
		<< QPointF(0.0, 20.0) << QPointF(0.0, 30.0)
		<< QPointF(0.0, 40.0) << QPointF(0.0, 50.0)
		<< QPointF(0.0, 60.0) << QPointF(0.0, 70.0)
		<< QPointF(0.0, 80.0) << QPointF(0.0, 90.0);
//		<< QPointF(0.0, 100.0);

	*m_series_Pon << QPointF(0.0, 85.0)  /*<< QPointF(10.0, 90.0)
										 << QPointF(10.0, 85.0) << QPointF(50.0, 85.0)
										 << QPointF(50.0, 80.0) << QPointF(70.0, 80.0)
										 << QPointF(70.0, 90.0) */ << QPointF(100.0, 85.0);

	*m_series_Poff << QPointF(0.0, 75.0) /*<< QPointF(30.0, 70.0)
										 << QPointF(30.0, 80.0) << QPointF(40.0, 80.0)
										 << QPointF(40.0, 70.0) << QPointF(70.0, 70.0)
										 << QPointF(70.0, 75.0) */ << QPointF(100.0, 75.0);

	*m_series_V_switch << QPointF(0.0, 65.0) << QPointF(100.0, 65.0);

	*m_series_V_recirc << QPointF(0.0, 55.0) << QPointF(100.0, 55.0);

	*m_series_solution << QPointF(0.0, 45.0) << QPointF(100.0, 45.0);

	*m_series_ask << QPointF(0.0, 35.0) /*<< QPointF(20.0, 20.0)
										<< QPointF(20.0, 30.0) << QPointF(50.0, 30.0)
										<< QPointF(50.0, 20.0) << QPointF(70.0, 20.0)
										<< QPointF(70.0, 30.0) */ << QPointF(100.0, 35.0);

	*m_series_sync_in << QPointF(0.0, 25.0) /*<< QPointF(30.0, 10.0)
											<< QPointF(30.0, 20.0) << QPointF(40.0, 20.0)
											<< QPointF(40.0, 20.0) << QPointF(70.0, 20.0)
											<< QPointF(70.0, 10.0)*/ << QPointF(100.0, 25.0);

	*m_series_sync_out << QPointF(0.0, 15.0) /*<< QPointF(20.0, 0.0)
											<< QPointF(20.0, 10.0) << QPointF(50.0, 10.0)
											<< QPointF(50.0, 0.0) << QPointF(70.0, 00.0)
											<< QPointF(70.0, 10.0)*/ << QPointF(100.0, 15.0);

	*m_time_line_b << QPointF(0.0, 0.0) << QPointF(0.0, 100.0);
	*m_time_line_t << QPointF(1.0, 0.0) << QPointF(1.0, 100.0);

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
	gradient.setColorAt(0.0, 0xAAAAAA);
	gradient.setColorAt(1.0, 0xFFFFFF);
	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	m_past_time_area->setBrush(gradient);
	//![2]

	//![3]
	QPen pen(0x059605);
	pen.setWidth(3);

	QLinearGradient gradient1(QPointF(0, 0), QPointF(0, 1));
	gradient1.setColorAt(0.0, 0x3cc63c);
	gradient1.setColorAt(1.0, 0x26f626);
	gradient1.setCoordinateMode(QGradient::ObjectBoundingMode);
	//series1A->setBrush(gradient1);
	QLinearGradient gradient2(QPointF(0, 0), QPointF(0, 1));
	gradient2.setColorAt(0.0, 0xc63c3c);
	gradient2.setColorAt(1.0, 0xf62626);
	gradient2.setCoordinateMode(QGradient::ObjectBoundingMode);
	//series2A->setBrush(gradient2);
	//![3]



	//![4]
	m_chart = new QtCharts::QChart();
	m_chart->legend()->hide();
	m_chart->addSeries(m_past_time_area);
	m_chart->addSeries(m_series_Pon);
	m_chart->addSeries(m_series_Poff);
	m_chart->addSeries(m_series_V_switch);
	m_chart->addSeries(m_series_V_recirc);
	m_chart->addSeries(m_series_solution);
	m_chart->addSeries(m_series_ask);
	m_chart->addSeries(m_series_sync_in);
	m_chart->addSeries(m_series_sync_out);
	m_chart->addSeries(m_time_line_b);
	m_chart->addSeries(m_time_line_t);




	//  chart->setTitle("Simple areachart example");
	//  chart->createDefaultAxes();
	QtCharts::QCategoryAxis *axisX = new QtCharts::QCategoryAxis();
	QtCharts::QCategoryAxis *axisY = new QtCharts::QCategoryAxis();

	// Customize axis label font
	QFont labelsFont;
	labelsFont.setPixelSize(12);
	axisX->setLabelsFont(labelsFont);
	axisX->setLabelsPosition(QtCharts::QCategoryAxis::AxisLabelsPositionOnValue);
	axisY->setLabelsFont(labelsFont);
	//axisY->setLabelsPosition(QtCharts::QCategoryAxis::AxisLabelsPositionOnValue);

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

	//axisX->append("10", 10);
	axisX->append("20 %", 20);
	//axisX->append("30", 30);
	axisX->append("40 %", 40);
	//axisX->append("50", 50);
	axisX->append("60 %", 60);
	//axisX->append("70", 70);
	axisX->append("80 %", 80);
	//axisX->append("90", 90);
	axisX->append("100 %", 100);
	axisX->setRange(0, 100);


	axisX->setTitleText("Simulation time percentage");

	//axisY->append(" ", 10);
	axisY->append("Sync Out", 20);
	axisY->append("Sync In", 30);
	axisY->append("Ask", 40);
	axisY->append("Solution", 50);
	axisY->append("V_recirc", 60);
	axisY->append("V_switch", 70);
	axisY->append("P_off", 80);
	axisY->append("P_on", 90);
	axisY->setRange(10, 90);

	m_chart->setAxisX(axisX, m_series_X);
	m_chart->setAxisY(axisY, m_series_Pon);
	m_chart->setAxisY(axisY, m_series_Poff);
	m_chart->setAxisY(axisY, m_series_V_switch);
	m_chart->setAxisY(axisY, m_series_V_recirc);
	m_chart->setAxisY(axisY, m_series_solution);
	m_chart->setAxisY(axisY, m_series_ask);
	m_chart->setAxisY(axisY, m_series_sync_in);
	m_chart->setAxisY(axisY, m_series_sync_out);
	m_chart->setAxisX(axisX, m_past_time_area);
	//chart->setAxisX(axisY, m_time_line_b);
	//chart->setAxisX(axisY, m_time_line_t);
	//chart->createDefaultAxes();
	m_chart->setMargins(QMargins(0, 0, 8, 0));
	m_chart->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));
	//chart->setContentsMargins(0, 0, 0, 0);

	//chart->axisX()->setRange(0, 100);
	//chart->axisY()->setRange(0, 100);
	//![4]

	//![5]
	m_chartView = new QtCharts::QChartView(m_chart);
	m_chartView->setRenderHint(QPainter::Antialiasing);
	m_chartView->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));
	//chartView->setContentsMargins(0, 0, 0, 0);
	//![5]

	//ui->gridLayout_12->addWidget(chartView);


}