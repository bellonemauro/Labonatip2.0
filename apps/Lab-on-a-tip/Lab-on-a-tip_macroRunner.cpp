/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip_macroRunner.h"


Labonatip_macroRunner::Labonatip_macroRunner(QMainWindow *parent ) :
	m_number_of_iterations(100),
	m_ppc1(NULL),
	m_macro(NULL),
	m_simulation_only(true),
	m_threadTerminationHandler(false)
{
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< " macroRunner initialization " << endl;
}

void Labonatip_macroRunner::run()  {
	QString result;
	m_threadTerminationHandler = true;
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< " Macro running " << endl;
	//QMessageBox::information(this, "Warning !", " Macro running. ");

	try
	{

		if (m_ppc1 && m_macro)
		{
			cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  " << " macro size " << m_macro->size() << endl;
			for (int i = 0; i < m_macro->size(); i++)
			{
				if (!m_threadTerminationHandler) {
					result = " MACRO STOPPED ";
					emit resultReady(result);
					return;
				}

				//cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  " << " i'm in the thread ... index " << i << endl;

				if (m_simulation_only)
				{

					QString message = QString::fromStdString(m_macro->at(i).status_message);
					message.append(" >>> command :  ");
					message.append(QString::number(m_macro->at(i).comando));
					message.append(" value ");
					message.append(QString::number(m_macro->at(i).value));
					message.append(" status message ");
					message.append(QString::fromStdString(m_macro->at(i).status_message));

					//message.append(" v_switch = ");
					//message.append(QString::number(m_macro->at(i).V_switch));
					//message.append(";  V_recirc = ");
					//message.append(QString::number(m_macro->at(i).V_recirc));
					//message.append(";  P_on = ");
					//message.append(QString::number(m_macro->at(i).P_on));
					//message.append(";  P_off = ");
					//message.append(QString::number(m_macro->at(i).P_off));
					//message.append(";  Valve a = ");
					//message.append(QString::number(m_macro->at(i).open_valve_a));
					//message.append(";  Valve b = ");
					//message.append(QString::number(m_macro->at(i).open_valve_b));
					//message.append(";  Valve c = ");
					//message.append(QString::number(m_macro->at(i).open_valve_c));
					//message.append(";  Valve d = ");
					//message.append(QString::number(m_macro->at(i).open_valve_d));

					emit sendStatusMessage(message);

					sleep(m_macro->at(i).Duration);
				}
				else {
					if (m_ppc1->isRunning()) {
						//cout  << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  " << " ppc1 is running the command " << m_macro->at(i).status_message << endl;
						//m_ppc1->setPressureChannelD(100);

						if (m_ppc1->m_PPC1_data->channel_A->set_point != m_macro->at(i).V_switch)
							m_ppc1->setVacuumChannelA(m_macro->at(i).V_switch);
						if (m_ppc1->m_PPC1_data->channel_B->set_point != m_macro->at(i).V_recirc)
							m_ppc1->setVacuumChannelB(m_macro->at(i).V_recirc);
						if (m_ppc1->m_PPC1_data->channel_C->set_point != m_macro->at(i).P_on)
							m_ppc1->setPressureChannelC(m_macro->at(i).P_on);
						if (m_ppc1->m_PPC1_data->channel_D->set_point != m_macro->at(i).P_off)
							m_ppc1->setPressureChannelD(m_macro->at(i).P_off);

						m_ppc1->setValve_l(m_macro->at(i).open_valve_a);
						m_ppc1->setValve_k(m_macro->at(i).open_valve_b);
						m_ppc1->setValve_j(m_macro->at(i).open_valve_c);
						m_ppc1->setValve_i(m_macro->at(i).open_valve_d);
						QString message = QString::fromStdString(m_macro->at(i).status_message);
						emit sendStatusMessage(message);

						sleep(m_macro->at(i).Duration);
						//m_ppc1->setPressureChannelD(0);
					}
					else {
						cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
							<< " Labonatip_macroRunner::run  ---- error --- MESSAGE: ppc1 is NOT running " << endl;
						result = " ppc1 is NOT running ";

						emit resultReady(result);
						return;
					}
				}
			}
		}
		else {
			cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
				<< " Labonatip_macroRunner::run  ---- error --- MESSAGE: null pointer " << endl;
			result = " null pointer ";

			emit resultReady(result);
			return;
		}
		result = " Success";

		emit resultReady(result);
		return;

	}
	catch(serial::IOException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: IOException : " << e.what() << endl;
		//m_ppc1->disconnectCOM();
		result = " failed"; 
		emit resultReady(result);
		return;
	}
	catch (serial::PortNotOpenedException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: PortNotOpenedException : " << e.what() << endl;
		//m_PPC1_serial->close();
		result = " failed";
		emit resultReady(result);
		return;
	}
	catch (serial::SerialException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: SerialException : " << e.what() << endl;
		//m_PPC1_serial->close();
		result = " failed";
		emit resultReady(result);
		return;
	}
	catch (exception &e) {
		cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< " Labonatip_GUI::disCon ::: Unhandled Exception: " << e.what() << endl;
		//m_PPC1_serial->close();
		result = " failed";
		emit resultReady(result);
		return;
	}



}

Labonatip_macroRunner::~Labonatip_macroRunner ()
{

}
