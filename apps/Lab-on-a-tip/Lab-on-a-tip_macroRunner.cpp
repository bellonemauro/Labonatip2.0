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
			// compute the duration of the macro
			double macro_duration = 0.0;
			for (int i = 0; i < m_macro->size(); i++) {
				if (m_macro->at(i).getInstruction() ==
					fluicell::PPC1api::command::instructions::sleep)
					macro_duration += m_macro->at(i).getValue();
			}
			double time_elapsed = 0.0;

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

					QString message = QString::fromStdString(m_macro->at(i).getStatusMessage());
					message.append(" >>> command :  ");
					message.append(QString::fromStdString(m_macro->at(i).getCommandAsString()));
					message.append(" value ");
					message.append(QString::number(m_macro->at(i).getValue()));
					message.append(" status message ");
					message.append(QString::fromStdString(m_macro->at(i).getStatusMessage()));

					emit sendStatusMessage(message);

					//sleep(1);

					if (m_macro->at(i).getInstruction() ==
						fluicell::PPC1api::command::instructions::sleep) {

						for (int j = 0; j < static_cast<int>(m_macro->at(i).getValue()); j++) {
							sleep(1);// (m_macro->at(i).Duration);					
							time_elapsed = time_elapsed + 1.0;
							int status = int(100 * time_elapsed / macro_duration);
							
							emit timeStatus(status);
					}
					}

				}
				else {
					if (m_ppc1->isRunning()) {
						//cout  << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  " << " ppc1 is running the command " << m_macro->at(i).status_message << endl;
						if (m_macro->at(i).getInstruction() == // If the command is to wait, we do it here
							fluicell::PPC1api::command::instructions::sleep) {
							//TODO : check the time update with the real device 
							for (int j = 0; j < static_cast<int>(m_macro->at(i).getValue()); j++) {
								sleep(1);// (m_macro->at(i).Duration);					
								time_elapsed = time_elapsed + 1.0;
								int status = int(100 * time_elapsed / macro_duration);

								emit timeStatus(status);
							}
						}
						else m_ppc1->run(m_macro->at(i)); // otherwise we run the actual command on the PPC1
						
						if (m_macro->at(i).isStatusVisualized()) {
							QString message = QString::fromStdString(m_macro->at(i).getStatusMessage());
							emit sendStatusMessage(message);
						}
					}
					else {
						cerr << QDate::currentDate().toString().toStdString() << "  " 
							 << QTime::currentTime().toString().toStdString() << "  "
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
