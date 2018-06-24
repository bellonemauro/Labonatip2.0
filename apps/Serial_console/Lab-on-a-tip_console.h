/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef LAB_ON_A_TIP_CONSOLE_H_
#define LAB_ON_A_TIP_CONSOLE_H_

#include "ui_Lab-on-a-tip_console.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

using namespace std;

class Labonatip_console;

/** This console has been implemented to try serial communication between
* the Fluicell device and the Lab-on-a-tip application
* \note
*/
class Labonatip_console : public  QDialog
{
	Q_OBJECT

		signals :
	void getData(const QByteArray &data);

public:
	explicit Labonatip_console(QWidget *parent = nullptr);

	~Labonatip_console();

	void setLocalEchoEnabled(bool set) { localEchoEnabled = set; }

	void exit();

	private slots:

	void connectCOM();

	void disconnectCOM();

	void showPortInfo(int idx);

	void handleError(QSerialPort::SerialPortError error);

	void readData();

	void sendData();

	void emumeratePorts();

protected: //TODO: maybe the events are not necessary
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

private:
	bool localEchoEnabled;
	QSerialPort *serial;

protected:
	Ui::Labonatip_console *ui_console;    //--> the user interface
};

#endif /* LAB_ON_A_TIP_CONSOLE_H_ */