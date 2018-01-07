/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_macroRunner_H_
#define Labonatip_macroRunner_H_

// standard libraries
#include <iostream>
#include <string>

// Qt
#include <QMainWindow>
#include <QDateTime>

// QT threads
#include <qthread.h>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>

class Labonatip_macroRunner : public  QThread
{
	Q_OBJECT

public:

	explicit Labonatip_macroRunner(QMainWindow *parent = nullptr);
	
	void run() Q_DECL_OVERRIDE;

	void setDevice(fluicell::PPC1api *_ppc1) { m_ppc1 = _ppc1; }
	
	void setProtocolPrt(std::vector<fluicell::PPC1api::command> *_protocol) { m_protocol = _protocol; };

	void killMacro(bool _kill) { m_threadTerminationHandler = !_kill; }

	void setSimulationFlag(bool _sim_flag){ m_simulation_only = _sim_flag; }

	void askOkEvent(bool _ask_ok) { m_ask_ok = _ask_ok; }

signals:
	void resultReady(const QString &_s);
	void sendStatusMessage(const QString &_message);
	void sendAskMessage(const QString &_message);
	void timeStatus(const double &_time);

private: 

	fluicell::PPC1api *m_ppc1;                            //!< device to run the protocol 
	std::vector<fluicell::PPC1api::command> *m_protocol;  //!< protocol to run
	bool m_simulation_only;                               //!< true if simulation, false use the PPC1
	bool m_threadTerminationHandler;                      //!< true to terminate the macro
	bool m_ask_ok;

};


#endif /* Labonatip_macroRunner_H_ */
