/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
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
	m_str_stopped = tr("PROTOCOL STOPPED");
	m_str_not_connected = tr("PPC1 is NOT running, connect and try again");
}

void Labonatip_macroRunner::switchLanguage(QString _translation_file)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroRunner::switchLanguage " << endl;

	qApp->removeTranslator(&m_translator_runner);

	if (m_translator_runner.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_runner);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_macroRunner::switchLanguage   installTranslator" << endl;
	}

}

void Labonatip_macroRunner::runCommand(fluicell::PPC1api::command _cmd)
{

	int ist = _cmd.getInstruction();
	//TODO: remap as Zone size, Flow speed, Vacuum, Wait, Alloff, Solution 1-4, Pon, Poff, Vrecirc, V switch, all the rest.

	switch (ist)
	{
	case fluicell::PPC1api::command::instructions::setPon: { //setPon
		emit setPon(_cmd.getValue());
		msleep(50);
		return;
	}
	case fluicell::PPC1api::command::instructions::setPoff: {//setPoff
		emit setPoff(_cmd.getValue());
		msleep(50);
		return;

	}
	case fluicell::PPC1api::command::instructions::setVswitch: {//setVswitch
		emit setVs(-_cmd.getValue());
		msleep(50);
		return;

	}
	case fluicell::PPC1api::command::instructions::setVrecirc: {//setVrecirc
		emit setVr(-_cmd.getValue());
		msleep(50);
		return;

	}
	case fluicell::PPC1api::command::instructions::solution1: {//solution1
		emit solution1(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case fluicell::PPC1api::command::instructions::solution2: {//solution2
		emit solution2(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case fluicell::PPC1api::command::instructions::solution3: {//solution3
		emit solution3(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case fluicell::PPC1api::command::instructions::solution4: {//solution4
		emit solution4(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case fluicell::PPC1api::command::instructions::wait: {//sleep
		// this is done in the run()
		return;
	}
	case fluicell::PPC1api::command::instructions::ask_msg: {//ask_msg
		QString msg = QString::fromStdString(_cmd.getStatusMessage());
		emit sendAskMessage(msg); // send ask message event
		m_ask_ok = false;
		while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
			msleep(500);
		}
		return;
	}
	case fluicell::PPC1api::command::instructions::allOff: {//allOff	
		emit closeAll();
		return;
	}
	case fluicell::PPC1api::command::instructions::pumpsOff: {//pumpsOff
		emit pumpOff();
		return;
	}
	case fluicell::PPC1api::command::instructions::waitSync: {//waitSync //TODO
		return;

	}
	case fluicell::PPC1api::command::instructions::syncOut: {//syncOut //TODO
		return;
	}
	case fluicell::PPC1api::command::instructions::zoneSize: {//zoneSize
		emit setDropletSizeSIG(_cmd.getValue());
		return;
	}
	case fluicell::PPC1api::command::instructions::flowSpeed: {//flowSpeed
		emit setFlowSpeedSIG(_cmd.getValue());
		return;
	}
	case fluicell::PPC1api::command::instructions::vacuum: {//vacuum
		emit setVacuumSIG(_cmd.getValue());
		return;
	}
	case fluicell::PPC1api::command::instructions::loop: {//loop
		// this is not to be done here
		return;
	}
	default: {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< " fluicell::PPC1api::runCommand(command _cmd) :::"
			<< " Command NOT recognized "
			<< endl;
		return;
	}
	}

}

void Labonatip_macroRunner::run() 
{
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

					emit sendStatusMessage(message);

					//TODO : the simulation does not run delivery fields zone_size, vacuum, 
					runCommand(m_protocol->at(i));

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
									<< "Labonatip_macroRunner::run  ---- error --- MESSAGE:"
									<< " error in ppc1api PPC1api::runCommand" << endl;
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
							 << " Labonatip_macroRunner::run  ---- error --- MESSAGE:"
							 << " ppc1 is NOT running " << endl;

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