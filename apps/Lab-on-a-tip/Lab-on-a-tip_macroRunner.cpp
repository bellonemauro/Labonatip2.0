/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip_macroRunner.h"


Labonatip_macroRunner::Labonatip_macroRunner(QMainWindow *parent ) :
	m_number_of_iterations(100),
	m_ppc1(NULL),
	m_macro(NULL),
	m_threadTerminationHandler(false)
{
	cout << " macroRunner initialization " << endl;

}

void Labonatip_macroRunner::run()  {
	QString result;
	m_threadTerminationHandler = true;
	cout << " Macro running " << endl;

	if (m_ppc1 && m_macro)
	{
		cout << " macro size " << m_macro->size() << endl;
		for (int i = 0; i < m_macro->size(); i++)
		{
			if (!m_threadTerminationHandler) {
				result = " MACRO STOPPED ";
				emit resultReady(result);
				return;
			}

			//cout << " i'm in the thread ... index " << i << endl;
			sleep(1);
			if (m_ppc1->isRunning()) {
				//cout << " ppc1 is running the command " << m_macro->at(i).status_message << endl;
				//m_ppc1->setPressureChannelD(100);

				if (m_ppc1->m_PPC1_data->channel_A->set_point != m_macro->at(i).V_switch)
					m_ppc1->setVacuumChannelA(m_macro->at(i).V_switch);
				if (m_ppc1->m_PPC1_data->channel_B->set_point != m_macro->at(i).V_recirc)
					m_ppc1->setVacuumChannelB(m_macro->at(i).V_recirc);
				if (m_ppc1->m_PPC1_data->channel_C->set_point != m_macro->at(i).P_on)
					m_ppc1->setPressureChannelC(m_macro->at(i).P_on);
				if (m_ppc1->m_PPC1_data->channel_D->set_point != m_macro->at(i).P_off)
					m_ppc1->setPressureChannelD(m_macro->at(i).P_off);

				m_ppc1->setValve_a(m_macro->at(i).open_valve_a);
				m_ppc1->setValve_b(m_macro->at(i).open_valve_b);
				m_ppc1->setValve_c(m_macro->at(i).open_valve_c);
				m_ppc1->setValve_d(m_macro->at(i).open_valve_d);
				QString message = QString::fromStdString(m_macro->at(i).status_message);
				emit sendStatusMessage(message);

				sleep(m_macro->at(i).Duration);
				//m_ppc1->setPressureChannelD(0);
			}
			else {
				cerr << " Labonatip_macroRunner::run  ---- error --- MESSAGE: ppc1 is NOT running " << endl;
				result = " ppc1 is NOT running ";

				emit resultReady(result);
				return;
			}
		}
	}
	else {
		cerr << " Labonatip_macroRunner::run  ---- error --- MESSAGE: null pointer " << endl;
		result = " null pointer ";

		emit resultReady(result);
		return;
	}
	result = " Success";

	emit resultReady(result);
	return;
}

Labonatip_macroRunner::~Labonatip_macroRunner ()
{

}
