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
	
	initCustomStrings();

}


void Labonatip_macroRunner::initCustomStrings()
{
	//TODO : translate

	//setting custom strings to translate 
	m_str_success = tr("Success");
	m_str_failed = tr("Failed");
	m_str_stopped = tr(" PROTOCOL STOPPED ");
	m_str_not_connected = tr("PPC1 is NOT running, connect and try again");
}

void Labonatip_macroRunner::runCommand(fluicell::PPC1api::command _cmd)
{

	int ist = _cmd.getInstruction();

	switch (ist)
	{
	case 0: { //setPon
		emit setPon(_cmd.getValue());
		msleep(50);
		return;
	}
	case 1: {//setPoff
		emit setPoff(_cmd.getValue());
		msleep(50);
		return;

	}
	case 2: {//setVswitch
		emit setVs(-_cmd.getValue());
		msleep(50);
		return;

	}
	case 3: {//setVrecirc
		emit setVr(-_cmd.getValue());
		msleep(50);
		return;

	}
	case 4: {//solution1
		emit solution1(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case 5: {//solution2
		emit solution2(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case 6: {//solution3
		emit solution3(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case 7: {//solution4
		emit solution4(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case 8: {//sleep
//		std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(_cmd.getValue())));
		return;
	}
	case 9: {//ask_msg
		return;
	}
	case 10: {//allOff	
		emit closeAll();
		return;
	}
	case 11: {//pumpsOff
		emit pumpOff();
		return;
	}
	case 12: {//waitSync //TODO
			  //waitsync(front type : can be : RISE or FALL), protocol stops until trigger signal is received
//		bool state;
//		if (_cmd.getValue() == 0) state = false;
//		else state = true;
		// reset the sync signals and then wait for the correct state to come
//		resetSycnSignals(false);
//		std::this_thread::sleep_for(std::chrono::milliseconds(20));
//		clock_t begin = clock();
//		while (!syncSignalArrived(state))
//		{
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//			clock_t end = clock();
//			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//			if (elapsed_secs > m_wait_sync_timeout) // break if timeout
//			{
//				cerr << currentDateTime()
//					<< " fluicell::PPC1api::run(command _cmd) ::: waitSync timeout "
//					<< endl;
//				return false;
//			}
//		}
//		return true;

	}
	case 13: {//syncOut //TODO
			  // syncout(int: pulse length in ms) if negative then default state is 1
			  // and pulse is 0, if positive, then pulse is 1 and default is 0
//		int v = static_cast<int>(_cmd.getValue());
//		if (m_verbose) cout << currentDateTime()
//			<< " fluicell::PPC1api::run(command _cmd) ::: "
//			<< "syncOut NOT implemented in the API ::: test value = "
//			<< v << endl;
//		int current_ppc1out_status = m_PPC1_data->ppc1_OUT;
//		bool success = setPulsePeriod(v);
//		std::this_thread::sleep_for(std::chrono::milliseconds(v));
		//TODO : this function is unsafe, in case the protocol is stop during this function, 
		//       the stop will not work
		/*
		clock_t begin = clock();
		while (current_ppc1out_status == m_PPC1_data->ppc1_OUT)
		{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		while (current_ppc1out_status != m_PPC1_data->ppc1_OUT)
		{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;*/
//		return success;
	}
	case 14: {//dropletSize
//		if (m_verbose) cout << currentDateTime()
//			<< " fluicell::PPC1api::run(command _cmd) ::: dropletSize  NOT entirely implemented in the API"
//			<< _cmd.getValue() << endl;
//		return setDropletSize(_cmd.getValue());
	}
	case 15: {//flowSpeed
//		if (m_verbose) cout << currentDateTime()
//			<< " fluicell::PPC1api::run(command _cmd) ::: flowSpeed  NOT entirely implemented in the API"
//			<< _cmd.getValue() << endl;
//		return setFlowspeed(_cmd.getValue());
	}
	case 16: {//vacuum

//		if (m_verbose) cout << currentDateTime()
//			<< " fluicell::PPC1api::run(command _cmd) ::: vacuum  "
//			<< _cmd.getValue() << endl;
//		return setVacuumPercentage(_cmd.getValue());
	}
	case 17: {//loop
//		if (m_verbose) cout << currentDateTime()
//			<< " fluicell::PPC1api::run(command _cmd) :::"
//			<< " loop NOT implemented in the API "
//			<< endl;

//		return true;
	}
	default: {
//		cerr << currentDateTime()
//			<< " fluicell::PPC1api::run(command _cmd) :::"
//			<< " Command NOT recognized "
//			<< endl;
//		return false;
	}
	}
		

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
					result = m_str_stopped; 
					emit resultReady(result);
					return;
				}

				//cout << QDate::currentDate().toString().toStdString() << "  "
				//       << QTime::currentTime().toString().toStdString() << "  " 
				//       << " I'm in the thread ... index " << i << endl;

				if (m_simulation_only)
				{

					QString message = QString::fromStdString(m_protocol->at(i).getStatusMessage());
					message.append(" >>> command :  ");
					message.append(QString::fromStdString(m_protocol->at(i).getCommandAsString()));
					message.append(" value ");
					message.append(QString::number(m_protocol->at(i).getValue()));
					message.append(" status message ");
					message.append(QString::fromStdString(m_protocol->at(i).getStatusMessage()));

					//TODO : the simulation does not give the actual commands,
					//       only messages are out ! 
					emit sendStatusMessage(message);

					runCommand(m_protocol->at(i));

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
							// visualize step time left 
							m_time_left_for_step = val - j;
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
								result = m_str_stopped; 
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
									result = m_str_stopped; 
									emit resultReady(result);
									return;
								}
							}
						}
						/*if (m_protocol->at(i).getInstruction() == // If the command is to wait, we do it here
							fluicell::PPC1api::command::instructions::waitSync) {

							bool state;
							if (int val = static_cast<int>(m_protocol->at(i).getValue()) == 0) state = false;
							else state = true;
							m_ppc1->resetSycnSignals(false);
							std::this_thread::sleep_for(std::chrono::milliseconds(20));
							clock_t begin = clock();
							while (!m_ppc1->syncSignalArrived(state))
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(1));
								clock_t end = clock();
								double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
								if (elapsed_secs > 200) // break if timeout
									break;
							}

						}*/
						else {
							if (!m_ppc1->runCommand(m_protocol->at(i))) // otherwise we run the actual command on the PPC1 
							{
								cerr << QDate::currentDate().toString().toStdString() << "  "
									<< QTime::currentTime().toString().toStdString() << "  "
									<< "Labonatip_macroRunner::run  ---- error --- MESSAGE: error in ppc1api PPC1api::runCommand" << endl;
							}
							else
							{
								//msleep(1); //TODO: this must be removed !
							}
						}
						//if (m_protocol->at(i).isStatusVisualized()) {
						//	QString message = QString::fromStdString(m_protocol->at(i).getStatusMessage());
						//	emit sendStatusMessage(message);
						//}
					}
					else {
						cerr << QDate::currentDate().toString().toStdString() << "  " 
							 << QTime::currentTime().toString().toStdString() << "  "
							 << " Labonatip_macroRunner::run  ---- error --- MESSAGE: ppc1 is NOT running " << endl;

						result = m_str_not_connected; 
						emit resultReady(result);
						return;
					}
				}
			}//end for protocol
		}
		else {
			cerr << QDate::currentDate().toString().toStdString() << "  " 
				 << QTime::currentTime().toString().toStdString() << "  "
				 << " Labonatip_macroRunner::run  ---- error --- MESSAGE: null pointer " << endl;
			result = m_str_failed; 

			emit resultReady(result);
			return;
		}
		result = m_str_success; 
		emit resultReady(result);
				
		return;

	}
	catch(serial::IOException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: IOException : " << e.what() << endl;
		//m_ppc1->disconnectCOM();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (serial::PortNotOpenedException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: PortNotOpenedException : " << e.what() << endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (serial::SerialException &e)
	{
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: SerialException : " << e.what() << endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (exception &e) {
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Labonatip_GUI::disCon ::: Unhandled Exception: " << e.what() << endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	
}