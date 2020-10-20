/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
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
	m_time_left_for_step(0),
	m_protocol_duration(0.0),
    m_time_elapsed(0.0)
{
	std::cout << HERE << std::endl;	
	initCustomStrings();
}


void Labonatip_macroRunner::initCustomStrings()
{
	//setting custom strings to translate 
	m_str_success = tr("Success");
	m_str_failed = tr("Failed");
	m_str_stopped = tr("PROTOCOL STOPPED");
	m_str_not_connected = tr("PPC1 is NOT running, connect and try again");
}

void Labonatip_macroRunner::switchLanguage(QString _translation_file)
{
	std::cout << HERE << std::endl;

	qApp->removeTranslator(&m_translator_runner);

	if (m_translator_runner.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_runner);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		std::cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_macroRunner::switchLanguage   installTranslator" << std::endl;
	}
}

void Labonatip_macroRunner::simulateCommand(fluicell::PPC1api6dataStructures::command _cmd)
{

	int ist = _cmd.getInstruction();

	switch (ist)
	{
	case pCmd::setPon: { //setPon
		emit setPon(_cmd.getValue());
		msleep(50);
		return;
	}
	case pCmd::setPoff: {//setPoff
		emit setPoff(_cmd.getValue());
		msleep(50);
		return;

	}
	case pCmd::setVswitch: {//setVswitch
		emit setVs(-_cmd.getValue());
		msleep(50);
		return;

	}
	case pCmd::setVrecirc: {//setVrecirc
		emit setVr(-_cmd.getValue());
		msleep(50);
		return;

	}
	case pCmd::solution1: {//solution1
		emit solution1(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case pCmd::solution2: {//solution2
		emit solution2(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case pCmd::solution3: {//solution3
		emit solution3(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case pCmd::solution4: {//solution4
		emit solution4(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case pCmd::solution5: {//solution4
		emit solution5(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case pCmd::solution6: {//solution4
		emit solution6(bool(_cmd.getValue()));
		msleep(50);
		return;
	}
	case pCmd::wait: {//sleep
		int val = static_cast<int>(_cmd.getValue());
		simulateWait(val);
		return;
	}
	case pCmd::ask_msg: {//ask_msg
		QString msg = QString::fromStdString(_cmd.getStatusMessage());
		emit sendAskMessage(msg); // send ask message event
		m_ask_ok = false;
		while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
			msleep(500);
		}
		return;
	}
	case pCmd::allOff: {//allOff	
		emit closeAll();
		return;
	}
	case pCmd::pumpsOff: {//pumpsOff
		emit pumpOff();
		return;
	}
	case pCmd::waitSync: {//waitSync
		// nothing is really to be done in simulation
		return;
	}
	case pCmd::syncOut: {//syncOut 
		// nothing is really to be done in simulation
		return;
	}
	case pCmd::setZoneSize: {//zoneSize
		emit setDropletSizeSIG(_cmd.getValue());
		return;
	}
	case pCmd::changeZoneSizeBy: {//changeZoneSize
		emit changeDropletSizeSIG(_cmd.getValue());
		return;
	}
	case pCmd::setFlowSpeed: {//flowSpeed
		emit setFlowSpeedSIG(_cmd.getValue());
		return;
	}
	case pCmd::changeFlowSpeedBy: {//changeFlowSpeed
		emit changeFlowSpeedSIG(_cmd.getValue());
		return;
	}
	case pCmd::setVacuum: {//vacuum
		emit setVacuumSIG(_cmd.getValue());
		return;
	}
	case pCmd::changeVacuumBy: {//vacuum
		emit changeVacuumSIG(_cmd.getValue());
		return;
	}
	case pCmd::loop: {//loop
		// this is not to be done here
		return;
	}
	default: {
		std::cerr << HERE
			<< " fluicell::PPC1api6::runCommand(command _cmd) :::"
			<< " Command NOT recognized "
			<< std::endl;
		return;
	}
	}
}


void Labonatip_macroRunner::simulateWait(int _sleep_for)
{
	const qint64 kInterval = 1000;
	qint64 mtime = QDateTime::currentMSecsSinceEpoch();
	for (int j = 0; j < _sleep_for; j++) {
		// visualize step time left 
		m_time_left_for_step = _sleep_for - j;
		mtime += kInterval;
		qint64 sleepFor = mtime - QDateTime::currentMSecsSinceEpoch();
		if (sleepFor < 0) {
			sleepFor = kInterval - ((-sleepFor) % kInterval);
		}
		msleep(sleepFor);// (m_macro->at(i).Duration);					
		m_time_elapsed = m_time_elapsed + 1.0;
		double status = 100.0 * m_time_elapsed / m_protocol_duration;

		// send the time status to the GUI
		emit timeStatus(status);

		// stop the wait function in case of thread termination handler
		if (!m_threadTerminationHandler) {
			//QString result = m_str_stopped;
			emit resultReady(m_str_stopped);
			return;
		}
	}

}


void Labonatip_macroRunner::run() 
{
	std::cout << HERE << std::endl;
	
	QString result;
	m_threadTerminationHandler = true;

	//main thread
	try
	{
		// the ppc1api and protocol must be initialized 
		if (m_ppc1 && m_protocol)
		{
			std::cout << HERE  << " protocol size " << m_protocol->size() << std::endl;

			// compute the duration of the macro
			m_protocol_duration = m_ppc1->protocolDuration(*m_protocol);
			m_time_elapsed = 0.0;

			// for all the commands in the protocol
			for (size_t i = 0; i < m_protocol->size(); i++)
			{
				// if we get the terminationHandler the thread is stopped
				if (!m_threadTerminationHandler) {
					result = m_str_stopped; 
					emit resultReady(result);
					return;
				}

				if (m_simulation_only)
				{
					// in simulation we set the status message
					QString message = QString::fromStdString(m_protocol->at(i).getStatusMessage());
					message.append(" >>> command :  ");
					message.append(QString::fromStdString(m_protocol->at(i).getCommandAsString()));
					message.append(" value ");
					message.append(QString::number(m_protocol->at(i).getValue()));
					message.append(" status message ");
					message.append(QString::fromStdString(m_protocol->at(i).getStatusMessage()));
					emit sendStatusMessage(message);

					// the command is simulated
					simulateCommand(m_protocol->at(i));

				}// end simulation only
				else {
					// if we are not in simulation and the ppc1 is running 
					// the commands will be sent to the ppc1 api
					if (m_ppc1->isRunning()) {
						
						// at GUI level only ask_msg and wait are handled
						if (m_protocol->at(i).getInstruction() ==
							pCmd::ask_msg) {
							QString msg = QString::fromStdString(m_protocol->at(i).getStatusMessage());

							emit sendAskMessage(msg); // send ask message event
							m_ask_ok = false;
							while (!m_ask_ok) {  // wait until the signal ok is pressed on the GUI
								msleep(500);
							}
						}

						// If the command is to wait, we do it here
						if (m_protocol->at(i).getInstruction() == pCmd::wait) 
						{	
							int val = static_cast<int>(m_protocol->at(i).getValue());
							simulateWait(val);							
						}//TODO: the waitSync works properly in the ppc1api, however, when the command is run
						 //      the ppc1api stops waiting for the signal and the GUI looks freezing without any message
						//if (m_protocol->at(i).getInstruction() == // If the command is to wait, we do it here
						//	pCmd::waitSync) {

						//	emit sendAskMessage("wait sync will run now another message will appear when the sync signal is detected");
						//	if (!m_ppc1->runCommand(m_protocol->at(i))) // otherwise we run the actual command on the PPC1 
						//	{
						//		cerr << HERE 
						//			<< " ---- error --- MESSAGE:"
						//			<< " error in ppc1api PPC1api::runCommand" << endl;
						//	}
						//	emit sendAskMessage("sync arrived");
						//}
						else {
							if (!m_ppc1->runCommand(m_protocol->at(i))) // otherwise we run the actual command on the PPC1 
							{
								std::cerr << HERE << " ---- error --- MESSAGE:"
									<< " error in ppc1api PPC1api::runCommand" << std::endl;
							}
						}
					}
					else {
						std::cerr << HERE << "  ---- error --- MESSAGE:"
							 << " ppc1 is NOT running " << std::endl;

						result = m_str_not_connected; 
						emit resultReady(result);
						return;
					}
				}
			}//end for protocol
		}
		else {
			std::cerr << HERE << "  ---- error --- MESSAGE: null pointer " << std::endl;
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
		std::cerr << HERE << " IOException : " << e.what() << std::endl;
		//m_ppc1->disconnectCOM();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (serial::PortNotOpenedException &e)
	{
		std::cerr << HERE << " PortNotOpenedException : " << e.what() << std::endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (serial::SerialException &e)
	{
		std::cerr << HERE << " SerialException : " << e.what() << std::endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	catch (std::exception &e) {
		std::cerr << HERE << " Unhandled Exception: " << e.what() << std::endl;
		//m_PPC1_serial->close();
		result = m_str_failed; 
		emit resultReady(result);
		return;
	}
	
}