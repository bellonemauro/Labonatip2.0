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

#include "Lab-on-a-tip.h"


// PPC1api test
#include <fluicell/ppc1api/ppc1api.h>

// QT threads
#include <qthread.h>
#include <qmutex.h>

using namespace std;



class Labonatip_macroRunner : public  QThread
{
	Q_OBJECT

public:

	explicit Labonatip_macroRunner(QMainWindow *parent = nullptr);
	
	~Labonatip_macroRunner();


	void run() Q_DECL_OVERRIDE;

	void setNumberOfIteration(int _number) { m_number_of_iterations = _number; }

	void setDevice(fluicell::PPC1api *_ppc1) { m_ppc1 = _ppc1; }
	
	void setMacroPrt(std::vector<fluicell::PPC1api::command> *_macro) { m_macro = _macro; };

	int m_number_of_iterations;

	void killMacro(bool _kill) { m_threadTerminationHandler = !_kill; }

	void setSimulationFlag(bool _sim_flag){ m_simulation_only = _sim_flag; }

	fluicell::PPC1api *m_ppc1;

signals:
	void resultReady(const QString &s);
	void sendStatusMessage(const QString &_message);

private: 

	std::vector<fluicell::PPC1api::command> *m_macro;
	bool m_simulation_only;
	bool m_threadTerminationHandler;

};


#endif /* Labonatip_macroRunner_H_ */
