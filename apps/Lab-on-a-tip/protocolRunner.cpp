/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolRunner.h"


Labonatip_macroRunner::Labonatip_macroRunner(QMainWindow *parent ) :
	m_ppc1(NULL),
	m_protocol(NULL),
	m_simulation_only(true),
	m_threadTerminationHandler(false),
	m_time_left_for_step(0)
{
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << " macroRunner initialization " << endl;
}

void Labonatip_macroRunner::run()  {
	QString result;
	m_threadTerminationHandler = true;
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << " Protocol running " << endl;
	
	try
	{

		if (m_ppc1 && m_protocol)
		{
			cout << QDate::currentDate().toString().toStdString() << "  " 
				 << QTime::currentTime().toString().toStdString() << "  " 
				 << " protocol size " << m_protocol->size() << endl;
			// compute the duration of the macro
			double macro_duration = 0.0;
			for (size_t i = 0; i < m_protocol->size(); i++) {
				if (m_protocol->at(i).getInstruction() ==
					fluicell::PPC1api::command::instructions::wait)
					macro_duration += m_protocol->at(i).getValue();
			}
			double time_elapsed = 0.0;

			for (size_t i = 0; i < m_protocol->size(); i++)
			{
				if (!m_threadTerminationHandler) {
					result = " PROTOCOL STOPPED ";
					emit resultReady(result);
					return;
				}

				//cout << QDate::currentDate().toString().toStdString() << "  "
				//       << QTime::currentTime().toString().toStdString() << "  " 
				//       << " i'm in the thread ... index " << i << endl;

				if (m_simulation_only)
				{

					QString message = QString::fromStdString(m_protocol->at(i).getStatusMessage());
					message.append(" >>> command :  ");
					message.append(QString::fromStdString(m_protocol->at(i).getCommandAsString()));
					message.append(" value ");
					message.append(QString::number(m_protocol->at(i).getValue()));
					message.append(" status message ");
					message.append(QString::fromStdString(m_protocol->at(i).getStatusMessage()));

					//TODO : the simulation does not give the actual commands, only messages are out ! 
					emit sendStatusMessage(message);

					if (m_protocol->at(i).getInstruction() ==
						fluicell::PPC1api::command::instructions::ask_msg) // if is askMsg we send the signals
					{
						QString msg = QString::fromStdString(m_protocol->at(i).getStatusMessage());
						emit sendAskMessage(msg); // send ask message event
						m_ask_ok = false;
						while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
							msleep(500);
						}
					}

					if (m_protocol->at(i).getInstruction() ==
						fluicell::PPC1api::command::instructions::wait) {
						int val = static_cast<int>(m_protocol->at(i).getValue());
						const qint64 kInterval = 1000;
						qint64 mtime = QDateTime::currentMSecsSinceEpoch();
						for (int j = 0; j < val; j++) {					
							mtime += kInterval;
							qint64 sleepFor = mtime - QDateTime::currentMSecsSinceEpoch();
							if (sleepFor < 0) {
								sleepFor = kInterval - ((-sleepFor) % kInterval);
							}
							msleep(sleepFor);// (m_macro->at(i).Duration);					
							time_elapsed = time_elapsed + 1.0;
							double status = 100.0 * time_elapsed / macro_duration;
														
							emit timeStatus(status);
							if (!m_threadTerminationHandler) {
								result = " PROTOCOL STOPPED ";
								emit resultReady(result);
								return;
							}
					}
					}

				}
				else {
					if (m_ppc1->isRunning()) {
						//cout  << QDate::currentDate().toString().toStdString() << "  "
						//      << QTime::currentTime().toString().toStdString() << "  " 
						//      << " ppc1 is running the command " << m_macro->at(i).status_message << endl;
						if (m_protocol->at(i).getInstruction() ==
							fluicell::PPC1api::command::instructions::ask_msg) {
							QString msg = QString::fromStdString(m_protocol->at(i).getStatusMessage());

							emit sendAskMessage(msg); // send ask message event
							m_ask_ok = false;
							while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
								msleep(500);
							}
						}
						if (m_protocol->at(i).getInstruction() == // If the command is to wait, we do it here
							fluicell::PPC1api::command::instructions::wait) {
							//TODO : check the time update with the real device 
							int val = static_cast<int>(m_protocol->at(i).getValue());
							const qint64 kInterval = 1000;
							qint64 mtime = QDateTime::currentMSecsSinceEpoch(); 
							for (int j = 0; j < val; j++) {
								// visualize step time left 
								m_time_left_for_step = val - j;

								// update the step
								mtime += kInterval;
								qint64 sleepFor = mtime - QDateTime::currentMSecsSinceEpoch();
								if (sleepFor < 0) {
									sleepFor = kInterval - ((-sleepFor) % kInterval);
								}
								msleep(sleepFor);// (m_macro->at(i).Duration);				
								time_elapsed = time_elapsed + 1.0;
								int status = int(100 * time_elapsed / macro_duration);
								


								emit timeStatus(status);
								if (!m_threadTerminationHandler) {
									result = " PROTOCOL STOPPED ";
									emit resultReady(result);
									return;
								}
							}
						}
						else m_ppc1->run(m_protocol->at(i)); // otherwise we run the actual command on the PPC1
						
						if (m_protocol->at(i).isStatusVisualized()) {
							QString message = QString::fromStdString(m_protocol->at(i).getStatusMessage());
							emit sendStatusMessage(message);
						}
					}
					else {
						cerr << QDate::currentDate().toString().toStdString() << "  " 
							 << QTime::currentTime().toString().toStdString() << "  "
							 << " Labonatip_macroRunner::run  ---- error --- MESSAGE: ppc1 is NOT running " << endl;

						result = " PPC1 is NOT running, connect and try again ";
						emit resultReady(result);
						return;
					}
				}
			}
		}
		else {
			cerr << QDate::currentDate().toString().toStdString() << "  " 
				 << QTime::currentTime().toString().toStdString() << "  "
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
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: IOException : " << e.what() << endl;
		//m_ppc1->disconnectCOM();
		result = " failed"; 
		emit resultReady(result);
		return;
	}
	catch (serial::PortNotOpenedException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: PortNotOpenedException : " << e.what() << endl;
		//m_PPC1_serial->close();
		result = " failed";
		emit resultReady(result);
		return;
	}
	catch (serial::SerialException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: SerialException : " << e.what() << endl;
		//m_PPC1_serial->close();
		result = " failed";
		emit resultReady(result);
		return;
	}
	catch (exception &e) {
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: Unhandled Exception: " << e.what() << endl;
		//m_PPC1_serial->close();
		result = " failed";
		emit resultReady(result);
		return;
	}
	
}