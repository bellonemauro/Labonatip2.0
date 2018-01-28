/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip.h"


void Labonatip_GUI::updateGUI() {

	if (!m_simulationOnly) {

		int sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_B->sensor_reading);  // rounded to second decimal
		m_pipette_status->v_switch_set_point = - m_ppc1->m_PPC1_data->channel_B->set_point;
		ui->label_switchPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(-m_pipette_status->v_switch_set_point)) + " mbar"));
		ui->progressBar_switch->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_A->sensor_reading);
		m_pipette_status->v_recirc_set_point = - m_ppc1->m_PPC1_data->channel_A->set_point;
		ui->label_recircPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(-m_pipette_status->v_recirc_set_point)) + " mbar"));
		ui->progressBar_recirc->setValue(-sensor_reading);

		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_C->sensor_reading);
		m_pipette_status->poff_set_point = m_ppc1->m_PPC1_data->channel_C->set_point;
		ui->label_PoffPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(m_pipette_status->poff_set_point)) + " mbar"));
		ui->progressBar_pressure_p_off->setValue(sensor_reading);

		sensor_reading = (int)(m_ppc1->m_PPC1_data->channel_D->sensor_reading);
		m_pipette_status->pon_set_point = m_ppc1->m_PPC1_data->channel_D->set_point;
		ui->label_PonPressure->setText(QString(QString::number(sensor_reading) +
			", " + QString::number(int(m_pipette_status->pon_set_point)) + " mbar"));
		ui->progressBar_pressure_p_on->setValue(sensor_reading);

		m_ds_perc = m_ppc1->getDropletSize();
		m_fs_perc = m_ppc1->getFlowSpeed();
		m_v_perc = m_ppc1->getVacuum();

		ui->lcdNumber_dropletSize_percentage->display(m_ppc1->getDropletSize());
		ui->lcdNumber_flowspeed_percentage->display(m_ppc1->getFlowSpeed());
		ui->lcdNumber_vacuum_percentage->display(m_ppc1->getVacuum());

		
		// check if some of the wells is open
		if (m_ppc1->m_PPC1_data->l == 1) {
			m_pen_line.setColor(m_sol1_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol1);

			// calculate the middle point between the two widget to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution1->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution1->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));
		}
		if (m_ppc1->m_PPC1_data->k == 1) {
			m_pen_line.setColor(m_sol2_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol2);

			// calculate the middle point between the two widget to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution2->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution2->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));
		}
		if (m_ppc1->m_PPC1_data->j == 1) {
			m_pen_line.setColor(m_sol3_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol3);

			// calculate the middle point between the two widget to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution3->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution3->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));
		}
		if (m_ppc1->m_PPC1_data->i == 1) {
			m_pen_line.setColor(m_sol4_color);
			// move the arrow in the drawing to point on the solution 4
			ui->widget_solutionArrow->setVisible(true);
			ui->label_arrowSolution->setText(m_solutionParams->sol4);

			// calculate the middle point between the two widget to align the arrow to the progressbar
			int pos_x = ui->progressBar_solution4->pos().x() -
				ui->widget_solutionArrow->width() / 2 +
				ui->progressBar_solution4->width() / 2;
			ui->widget_solutionArrow->move(
				QPoint(pos_x, ui->widget_solutionArrow->pos().ry()));
		}

		if (m_ppc1->m_PPC1_data->i == 0 && m_ppc1->m_PPC1_data->j == 0 &&
			m_ppc1->m_PPC1_data->k == 0 && m_ppc1->m_PPC1_data->l == 0) {
			m_pen_line.setColor(Qt::transparent);
			ui->widget_solutionArrow->setVisible(false);
		}
	}

	updateFlows();

	if (m_pipette_active) { 
		updateDrawing(m_ppc1->getDropletSize()); 
	}
	else {
		updateDrawing(ui->lcdNumber_dropletSize_percentage->value());
	}

	if (m_ppc1->isRunning()) {   //TODO check: update GUI only when we are running?
		m_update_GUI->start();
	}
}


void Labonatip_GUI::updateFlows()
{
	//shorter variables inside the function just for convinience
	double v_s = m_pipette_status->v_switch_set_point;
	double v_r = m_pipette_status->v_recirc_set_point;
	double p_on = m_pipette_status->pon_set_point;
	double p_off = m_pipette_status->poff_set_point;


	if (!m_simulationOnly) {  // if we are not in simulation, we just get the numbers from the PPC1api
		m_pipette_status->outflow_on = m_ppc1->m_PPC1_status->outflow_on;
		m_pipette_status->outflow_off = m_ppc1->m_PPC1_status->outflow_off; 
		m_pipette_status->outflow_tot = m_ppc1->m_PPC1_status->outflow_tot; 
		m_pipette_status->inflow_recirculation = m_ppc1->m_PPC1_status->inflow_recirculation;
		m_pipette_status->inflow_switch = m_ppc1->m_PPC1_status->inflow_switch;
		m_pipette_status->in_out_ratio_on = m_ppc1->m_PPC1_status->in_out_ratio_on;
		m_pipette_status->in_out_ratio_off = m_ppc1->m_PPC1_status->in_out_ratio_off;
		m_pipette_status->in_out_ratio_tot = m_ppc1->m_PPC1_status->in_out_ratio_tot;
		m_pipette_status->flow_well1 = m_ppc1->m_PPC1_status->flow_rate_1;
		m_pipette_status->flow_well2 = m_ppc1->m_PPC1_status->flow_rate_2;
		m_pipette_status->flow_well3 = m_ppc1->m_PPC1_status->flow_rate_3;
		m_pipette_status->flow_well4 = m_ppc1->m_PPC1_status->flow_rate_4;
		m_pipette_status->flow_well5 = m_ppc1->m_PPC1_status->flow_rate_5;
		m_pipette_status->flow_well6 = m_ppc1->m_PPC1_status->flow_rate_6;
		m_pipette_status->flow_well7 = m_ppc1->m_PPC1_status->flow_rate_7;
		m_pipette_status->flow_well8 = m_ppc1->m_PPC1_status->flow_rate_8;

	}
	else {
		// calculate inflow
		m_pipette_status->delta_pressure = 100.0 * v_r;
		m_pipette_status->inflow_recirculation = 2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, LENGTH_TO_TIP);

		m_pipette_status->delta_pressure = 100.0 * (v_r + 2.0 * p_off * (1 - LENGTH_TO_ZONE / LENGTH_TO_TIP));
		m_pipette_status->inflow_switch = 2 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, LENGTH_TO_TIP);

		m_pipette_status->delta_pressure = 100.0 * 2.0 * p_off;
		m_pipette_status->solution_usage_off = m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * LENGTH_TO_ZONE);

		m_pipette_status->delta_pressure = 100.0 * p_on;
		m_pipette_status->solution_usage_on = m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, LENGTH_TO_TIP);

		m_pipette_status->delta_pressure = 100.0 * (p_on + p_off * 3.0 - v_s * 2.0);   // TODO magic numbers
		m_pipette_status->outflow_on = m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, LENGTH_TO_TIP);

		m_pipette_status->delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);
		m_pipette_status->outflow_off = 2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * LENGTH_TO_ZONE);

		m_pipette_status->in_out_ratio_on = m_pipette_status->outflow_on / m_pipette_status->inflow_recirculation;
		if (isnan(m_pipette_status->in_out_ratio_on)) m_pipette_status->in_out_ratio_on = 0;

		m_pipette_status->in_out_ratio_off = m_pipette_status->outflow_off / m_pipette_status->inflow_recirculation;
		if (isnan(m_pipette_status->in_out_ratio_off)) m_pipette_status->in_out_ratio_off = 0;

		if (ui->pushButton_solution1->isChecked() ||
			ui->pushButton_solution2->isChecked() ||
			ui->pushButton_solution3->isChecked() ||
			ui->pushButton_solution4->isChecked()) // flow when solution is off // TODO : check on off
		{

			//m_pipette_status->delta_pressure = 100.0 * (p_on + p_off * 3.0 - v_s * 2.0);   // TODO magic numbers
			//m_pipette_status->outflow = m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, LENGTH_TO_TIP);

			
			m_pipette_status->in_out_ratio_tot = m_pipette_status->in_out_ratio_on;
			m_pipette_status->outflow_tot = m_pipette_status->outflow_on;

			if (ui->pushButton_solution1->isChecked()) m_pipette_status->flow_well1 = m_pipette_status->solution_usage_on;
			else m_pipette_status->flow_well1 = m_pipette_status->solution_usage_off;
			if (ui->pushButton_solution2->isChecked()) m_pipette_status->flow_well2 = m_pipette_status->solution_usage_on;
			else m_pipette_status->flow_well2 = m_pipette_status->solution_usage_off;
			if (ui->pushButton_solution3->isChecked()) m_pipette_status->flow_well3 = m_pipette_status->solution_usage_on;
			else m_pipette_status->flow_well3 = m_pipette_status->solution_usage_off;
			if (ui->pushButton_solution4->isChecked()) m_pipette_status->flow_well4 = m_pipette_status->solution_usage_on;
			else m_pipette_status->flow_well4 = m_pipette_status->solution_usage_off;

		}
		else // flow when solution is on // TODO : check on off
		{
			//m_pipette_status->delta_pressure = 100.0 * (p_off * 4.0 - v_s * 2.0);
			//m_pipette_status->outflow = 2.0 * m_ppc1->getFlowSimple(m_pipette_status->delta_pressure, 2 * LENGTH_TO_ZONE);

			m_pipette_status->in_out_ratio_tot = m_pipette_status->in_out_ratio_off;
			m_pipette_status->outflow_tot = m_pipette_status->outflow_off;

			m_pipette_status->flow_well1 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well2 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well3 = m_pipette_status->solution_usage_off;
			m_pipette_status->flow_well4 = m_pipette_status->solution_usage_off;

		}
		m_pipette_status->flow_well5 = m_pipette_status->inflow_switch / 2.0;
		m_pipette_status->flow_well6 = m_pipette_status->inflow_switch / 2.0;
		m_pipette_status->flow_well7 = m_pipette_status->inflow_recirculation / 2.0;
		m_pipette_status->flow_well8 = m_pipette_status->inflow_recirculation / 2.0;
	}

	// update the tree widget
	ui->treeWidget_macroInfo->topLevelItem(0)->setText(1,
		QString::number(m_pipette_status->outflow_tot, 'g', 3));
	ui->treeWidget_macroInfo->topLevelItem(1)->setText(1,
		QString::number(m_pipette_status->inflow_recirculation, 'g', 4));
	ui->treeWidget_macroInfo->topLevelItem(2)->setText(1,
		QString::number(m_pipette_status->inflow_switch, 'g', 4));
	ui->treeWidget_macroInfo->topLevelItem(3)->setText(1,
		QString::number(m_pipette_status->in_out_ratio_tot, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(4)->setText(1,
		QString::number(m_pipette_status->flow_well1, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(5)->setText(1,
		QString::number(m_pipette_status->flow_well2, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(6)->setText(1,
		QString::number(m_pipette_status->flow_well3, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(7)->setText(1,
		QString::number(m_pipette_status->flow_well4, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(8)->setText(1,
		QString::number(m_pipette_status->flow_well5, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(9)->setText(1,
		QString::number(m_pipette_status->flow_well6, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(10)->setText(1,
		QString::number(m_pipette_status->flow_well7, 'g', 2));
	ui->treeWidget_macroInfo->topLevelItem(11)->setText(1,
		QString::number(m_pipette_status->flow_well8, 'g', 2));

	return;
}



void Labonatip_GUI::updateDrawing(int _value) {


	if (_value == -1 || _value == 0) { // _value = -1 cleans the scene and make the flow disappear 

		m_scene_solution->clear();
		ui->graphicsView->update();
		ui->graphicsView->show();
		return;
	}

	//clean the scene
	m_scene_solution->clear();

	// draw the circle 
	QBrush brush(m_pen_line.color(), Qt::SolidPattern);

	QPen * border_pen = new QPen();
	border_pen->setColor(Qt::transparent);
	border_pen->setWidth(1);

	double droplet_modifier = (10.0 - _value / 10.0);
	// TODO: this is an attempt to make the droplet to look a little bit more realistic
	QPainterPath* droplet = new QPainterPath();
	droplet->arcMoveTo((qreal)ui->doubleSpinBox_d_x->value() + droplet_modifier, (qreal)ui->doubleSpinBox_d_y->value(),
		(qreal)ui->doubleSpinBox_d_w->value() - droplet_modifier, (qreal)ui->doubleSpinBox_d_h->value(), (qreal)ui->doubleSpinBox_d_a->value());
	
	droplet->arcTo((qreal)ui->doubleSpinBox_d2x->value() + droplet_modifier, (qreal)ui->doubleSpinBox_d2y->value(),
		(qreal)ui->doubleSpinBox_d2w->value() - droplet_modifier, (qreal)ui->doubleSpinBox_d2h->value(),
		(qreal)ui->doubleSpinBox_d2a->value(), (qreal)ui->doubleSpinBox_d2l->value());


	droplet->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*droplet, *border_pen, brush);

	//TODO: all this function is rather GUI fix stuff and the number should definitively be changed
	QPainterPath* circle = new QPainterPath();
	circle->arcMoveTo((qreal)ui->doubleSpinBox_c_x->value(), (qreal)ui->doubleSpinBox_c_y->value(),
		(qreal)ui->doubleSpinBox_c_w->value(), (qreal)ui->doubleSpinBox_c_h->value(),
		(qreal)ui->doubleSpinBox_c_a->value());  //TODO: a lot of magic numbers !!!! wow !

	circle->arcTo((qreal)ui->doubleSpinBox_c2x->value(), (qreal)ui->doubleSpinBox_c2y->value(),
		(qreal)ui->doubleSpinBox_c2w->value(), (qreal)ui->doubleSpinBox_c2h->value(),
		(qreal)ui->doubleSpinBox_c2a->value(), (qreal)ui->doubleSpinBox_c2l->value());

	circle->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*circle, *border_pen, brush);

	int border_pen_pipe_width = 7;
	QBrush brush_pipes(Qt::transparent, Qt::NoBrush);
	QPen * border_pen_pipe1 = new QPen();
	border_pen_pipe1->setColor(m_sol3_color); //TODO: fit the numbers of pipe solution with the colors !
	border_pen_pipe1->setWidth(border_pen_pipe_width);
	QPainterPath* path_pipe1 = new QPainterPath();
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);

	path_pipe1->arcMoveTo((qreal)ui->doubleSpinBox_p1_x->value(), (qreal)ui->doubleSpinBox_p1_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p1_w->value(), (qreal)ui->doubleSpinBox_p1_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p1_a->value());

	path_pipe1->arcTo((qreal)ui->doubleSpinBox_p12x->value(), (qreal)ui->doubleSpinBox_p12y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p12w->value(), (qreal)ui->doubleSpinBox_p12h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p12a->value(), (qreal)ui->doubleSpinBox_p12l->value()); //qreal startAngle, qreal arcLength


	path_pipe1->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*path_pipe1, *border_pen_pipe1, brush_pipes);

	QPen * border_pen_pipe2 = new QPen();
	border_pen_pipe2->setColor(m_sol1_color); //TODO: fit the numbers of pipe solution with the colors !
	border_pen_pipe2->setWidth(border_pen_pipe_width);
	QPainterPath* path_pipe2 = new QPainterPath();
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);


	path_pipe2->arcMoveTo((qreal)ui->doubleSpinBox_p2_x->value(), (qreal)ui->doubleSpinBox_p2_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p2_w->value(), (qreal)ui->doubleSpinBox_p2_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p2_a->value());
	path_pipe2->arcTo((qreal)ui->doubleSpinBox_p22x->value(), (qreal)ui->doubleSpinBox_p22y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p22w->value(), (qreal)ui->doubleSpinBox_p22h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p22a->value(), (qreal)ui->doubleSpinBox_p22l->value()); //qreal startAngle, qreal arcLength

	path_pipe2->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*path_pipe2, *border_pen_pipe2, brush_pipes);

	
	QPen * border_pen_pipe3 = new QPen();
	border_pen_pipe3->setColor(m_sol2_color);  //TODO: fit the numbers of pipe solution with the colors !
	border_pen_pipe3->setWidth(border_pen_pipe_width);
	QPainterPath* path_pipe3 = new QPainterPath();
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe3->arcMoveTo((qreal)ui->doubleSpinBox_p3_x->value(), (qreal)ui->doubleSpinBox_p3_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p3_w->value(), (qreal)ui->doubleSpinBox_p3_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p3_a->value());
	path_pipe3->arcTo((qreal)ui->doubleSpinBox_p32x->value(), (qreal)ui->doubleSpinBox_p32y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p32w->value(), (qreal)ui->doubleSpinBox_p32h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p32a->value(), (qreal)ui->doubleSpinBox_p32l->value()); //qreal startAngle, qreal arcLength
	path_pipe3->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*path_pipe3, *border_pen_pipe3, brush_pipes);

	
	QPen * border_pen_pipe4 = new QPen();
	border_pen_pipe4->setColor(m_sol4_color);
	border_pen_pipe4->setWidth(border_pen_pipe_width);
	QPainterPath* path_pipe4 = new QPainterPath();
	// void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);
	path_pipe4->arcMoveTo((qreal)ui->doubleSpinBox_p4_x->value(), (qreal)ui->doubleSpinBox_p4_y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p4_w->value(), (qreal)ui->doubleSpinBox_p4_h->value(),	// qreal w, qreal h, 
		(qreal)ui->doubleSpinBox_p4_a->value());

	path_pipe4->arcTo((qreal)ui->doubleSpinBox_p42x->value(), (qreal)ui->doubleSpinBox_p42y->value(),      // qreal x, qreal y,
		(qreal)ui->doubleSpinBox_p42w->value(), (qreal)ui->doubleSpinBox_p42h->value(),	// qreal w, qreal h,
		(qreal)ui->doubleSpinBox_p42a->value(), (qreal)ui->doubleSpinBox_p42l->value()); //qreal startAngle, qreal arcLength

	path_pipe4->setFillRule(Qt::FillRule::WindingFill);
	m_scene_solution->addPath(*path_pipe4, *border_pen_pipe4, brush_pipes);

	// draw a line from the injector to the solution release point 
	m_scene_solution->addLine(l_x1, l_y1, l_x2, l_y2, m_pen_line);

	ui->graphicsView->setScene(m_scene_solution);
	ui->graphicsView->show();

	return;
}


void Labonatip_GUI::updateWaste()  // this is updated every second
								   //TODO: this does not work with PPC1api (only works in simulation)
{
	//TODO: here there is a calculation of the volume as follows:
	//      remaining volume = current_volume (mL) - delta_t * flow_rate (nL/s)
	//      there is an assumption of this to run every second hence delta_t = 1  
	//TODO: check the conversions

	m_update_waste->start();

	if (m_ds_perc < 10) return;

	double waste_remaining_time_in_sec;

	updateWells();

	m_pipette_status->rem_vol_well5 = m_pipette_status->rem_vol_well5 +
		0.001 * m_pipette_status->flow_well5;
	m_pipette_status->rem_vol_well6 = m_pipette_status->rem_vol_well6 +
		0.001 * m_pipette_status->flow_well6;
	m_pipette_status->rem_vol_well7 = m_pipette_status->rem_vol_well7 +
		0.001 * m_pipette_status->flow_well7;
	m_pipette_status->rem_vol_well8 = m_pipette_status->rem_vol_well8 +
		0.001 * m_pipette_status->flow_well8;


	// only the minimum of the remaining solution is shown and important
	vector<double> v1;
	v1.push_back(m_solutionParams->vol_well5 - m_pipette_status->rem_vol_well5);
	v1.push_back(m_solutionParams->vol_well6 - m_pipette_status->rem_vol_well6);
	v1.push_back(m_solutionParams->vol_well7 - m_pipette_status->rem_vol_well7);
	v1.push_back(m_solutionParams->vol_well8 - m_pipette_status->rem_vol_well8);

	auto min = std::min_element(v1.begin(), v1.end());
	int min_index = std::distance(v1.begin(), min);

	switch (min_index)
	{
	case 0: { //TODO : the waste time is not well calculated 
		if (m_pipette_status->flow_well5 != 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well5 -
				m_pipette_status->rem_vol_well5) / m_pipette_status->flow_well5;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 1: {
		if (m_pipette_status->flow_well6 != 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well6 -
				m_pipette_status->rem_vol_well6) / m_pipette_status->flow_well6;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 2: {
		if (m_pipette_status->flow_well7 != 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well7 -
				m_pipette_status->rem_vol_well7) / m_pipette_status->flow_well7;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	case 3: {
		if (m_pipette_status->flow_well8 != 0) {
			waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well8 -
				m_pipette_status->rem_vol_well8) / m_pipette_status->flow_well8;
		}
		else {
			waste_remaining_time_in_sec = 0;
		}
		break;
	}
	default: {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_GUI::updateWaste  error --- no max found " << endl;
	}
	}

	int v = m_pipette_status->rem_vol_well1 * 10;
	double value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(12)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well2 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(13)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well3 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(14)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well4 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(15)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well5 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(16)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well6 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(17)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well7 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(18)->setText(1, QString::number(value));
	v = m_pipette_status->rem_vol_well8 * 10;
	value = v / 10.0;
	ui->treeWidget_macroInfo->topLevelItem(19)->setText(1, QString::number(value));

	value = 100 - (m_solutionParams->vol_well5 - m_pipette_status->rem_vol_well5) * 100 / m_solutionParams->vol_well5;
	ui->progressBar_switchOut->setValue(value);

	value = 100 - (m_solutionParams->vol_well6 - m_pipette_status->rem_vol_well6) * 100 / m_solutionParams->vol_well6;
	ui->progressBar_switchIn->setValue(value);

	value = 100 - (m_solutionParams->vol_well7 - m_pipette_status->rem_vol_well7) * 100 / m_solutionParams->vol_well7;
	ui->progressBar_recircOut->setValue(value);

	value = 100 - (m_solutionParams->vol_well8 - m_pipette_status->rem_vol_well8) * 100 / m_solutionParams->vol_well8;
	ui->progressBar_recircIn->setValue(value);


	if (waste_remaining_time_in_sec < 0) {
		toolEmptyWells();
		//TODO: what to do in this case?
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_GUI::updateWaste  error : Waste full ---- MB : WHAT TO DO? " << endl;
		//QMessageBox::information(this, "Warning !", " Waste full ---- MB : WHAT TO DO? ");
		return;
	}

	// build the string for the waste label
	QString s;
	s.append("Waste ");
	s.append(QString::number(min_index + 5));
	s.append(" full in: ");
	int remaining_hours = floor(waste_remaining_time_in_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor(((int)waste_remaining_time_in_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = waste_remaining_time_in_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	s.append(" h, \n");
	s.append(QString::number(remaining_mins));
	s.append(" min \n");
	s.append(QString::number(remaining_secs));
	s.append(" sec ");

	ui->textEdit_emptyTime_waste->setText(s);

}

void Labonatip_GUI::updateWells()
{
	int max_vol_in_well = 30;

	// update wells when the solution is flowing
	if (ui->pushButton_solution1->isChecked()) {
		m_pipette_status->rem_vol_well1 = m_pipette_status->rem_vol_well1 - //TODO: add check and block for negative values
			0.001 * m_pipette_status->flow_well1;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well1)
			/ max_vol_in_well;
		ui->progressBar_solution1->setValue(int(perc));

		// TODO: there is no check if the remaining solution is zero !

		//waste_remaining_time_in_sec = 1000.0 * (m_solutionParams->vol_well1 - // this is in micro liters 10^-6
		//	m_solutionParams->rem_vol_well1) /  //this is in micro liters 10^-6
		//	ui->treeWidget_macroInfo->topLevelItem(4)->text(1).toDouble(); // this is in nano liters 10^-9
		return;
	}
	if (ui->pushButton_solution2->isChecked()) {
		m_pipette_status->rem_vol_well2 = m_pipette_status->rem_vol_well2 -
			0.001 * m_pipette_status->flow_well2;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well2)
			/ max_vol_in_well;
		ui->progressBar_solution2->setValue(int(perc));
		return;
	}
	if (ui->pushButton_solution3->isChecked()) {
		m_pipette_status->rem_vol_well3 = m_pipette_status->rem_vol_well3 -
			0.001 * m_pipette_status->flow_well3;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well3)
			/ max_vol_in_well;
		ui->progressBar_solution3->setValue(int(perc));
		return;
	}
	if (ui->pushButton_solution4->isChecked()) {
		m_pipette_status->rem_vol_well4 = m_pipette_status->rem_vol_well4 -
			0.001 * m_pipette_status->flow_well4;

		double perc = 100.0 - 100.0 *
			(max_vol_in_well - m_pipette_status->rem_vol_well4)
			/ max_vol_in_well;
		ui->progressBar_solution4->setValue(int(perc));
		return;
	}

}