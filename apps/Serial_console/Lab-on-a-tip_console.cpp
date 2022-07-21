/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Lab-on-a-tip_console.h"

static const char blankString[] = QT_TRANSLATE_NOOP("Labonatip_console", "N/A");

Labonatip_console::Labonatip_console(QWidget *parent ):
									 QDialog (parent),
                                     ui_console (new Ui::Labonatip_console),
                                     localEchoEnabled(false),
									 serial(new QSerialPort(parent))
{
  ui_console->setupUi(this );

// initialize and connect serial port objects
  //serial = new QSerialPort(this);
  connect(serial, &QSerialPort::errorOccurred, this, &Labonatip_console::handleError);
  connect(serial, &QSerialPort::readyRead, this, &Labonatip_console::readData);

  // set serial port description for the first found port
  QString description;
  QString manufacturer;
  QString serialNumber;
  QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &info : infos) {
	  QStringList list;
	  description = info.description();
	  manufacturer = info.manufacturer();
	  serialNumber = info.serialNumber();
	  list << info.portName()
		  << (!description.isEmpty() ? description : blankString)
		  << (!manufacturer.isEmpty() ? manufacturer : blankString)
		  << (!serialNumber.isEmpty() ? serialNumber : blankString)
		  << info.systemLocation()
		  << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
		  << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

	  ui_console->comboBox_serialInfo->addItem(list.first(), list);
  }// TODO: in debug the detor of infos crushes

  // enable/disable GUI buttons
  ui_console->pushButton_connectCOM->setEnabled(true);
  ui_console->pushButton_disconnectCOM->setEnabled(false);
  ui_console->pushButton_sendMessage->setEnabled(false);
  ui_console->lineEdit_writeData->setEnabled(false);

  // connect buttons 
  connect(ui_console->pushButton_connectCOM, SIGNAL(clicked()), this, SLOT(connectCOM()));
  connect(ui_console->pushButton_disconnectCOM, SIGNAL(clicked()), this, SLOT(disconnectCOM()));
  connect(ui_console->pushButton_sendMessage, SIGNAL(clicked()), this, SLOT(sendData()));
  connect(ui_console->lineEdit_writeData, SIGNAL(returnPressed()), this, SLOT(sendData()));
  connect(ui_console->pushButton_enumerate, SIGNAL(clicked()), this, SLOT(emumeratePorts()));

  ui_console->comboBox_serialInfo->clear();
  connect(ui_console->comboBox_serialInfo,
	  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
	  &Labonatip_console::showPortInfo);

}

void Labonatip_console::connectCOM()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	ui_console->textEdit_console->append(" connecting COM ...  \n");
	serial->setPortName( ui_console->comboBox_serialInfo->currentText() ); //(comSettings->name);
	serial->setBaudRate( ui_console->comboBox_baudRate->currentText().toInt() );// (comSettings->baudRate);

	serial->setDataBits(static_cast<QSerialPort::DataBits>(ui_console->comboBox_dataBit->currentText().toInt()));
	serial->setParity(static_cast<QSerialPort::Parity>(ui_console->comboBox_parity->currentIndex()));
	serial->setStopBits(static_cast<QSerialPort::StopBits>(ui_console->comboBox_stopBit->currentIndex()));
	serial->setFlowControl(static_cast<QSerialPort::FlowControl>( ui_console->comboBox_flowControl->currentIndex()));

	if (serial->open(QIODevice::ReadWrite)) {
		// enable/disable GUI buttons
		ui_console->pushButton_connectCOM->setEnabled(false);
		ui_console->pushButton_disconnectCOM->setEnabled(true);
		ui_console->pushButton_sendMessage->setEnabled(true);
		ui_console->lineEdit_writeData->setEnabled(true);
		ui_console->groupBox_comSettings->setEnabled(false);
		QString ss(tr("Connected to %1 : %2, %3, %4, %5, %6 \n")
			.arg(serial->portName()).arg(serial->baudRate()).arg(serial->dataBits())
			.arg(serial->parity()).arg(serial->stopBits()).arg(serial->flowControl()));
		//cout << ss.toStdString() << endl;
		ui_console->textEdit_console->append(ss);
	}
	else {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QString ss(tr("Could not connect to %1 : %2, %3, %4, %5, %6 \n")
			.arg(serial->portName()).arg(serial->baudRate()).arg(serial->dataBits())
			.arg(serial->parity()).arg(serial->stopBits()).arg(serial->flowControl())); 
		ui_console->textEdit_console->append(ss);
		QMessageBox::critical(this, tr("Error"), serial->errorString());
		return;
		//cout << "Open error" << endl;
	}

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}

void Labonatip_console::disconnectCOM()
{
	if (serial->isOpen())
		serial->close();

	// enable/disable GUI buttons
	ui_console->pushButton_connectCOM->setEnabled(true);
	ui_console->pushButton_disconnectCOM->setEnabled(false);
	ui_console->pushButton_sendMessage->setEnabled(false);
	ui_console->lineEdit_writeData->setEnabled(false);
	ui_console->groupBox_comSettings->setEnabled(true);
	return;
}

void Labonatip_console::sendData( )
{
	QByteArray data;
	data = ui_console->lineEdit_writeData->text().toUtf8();
	data.append("\n");
	serial->write(data); 

	//cout << " sent string " << data.toStdString() << endl;
	ui_console->textEdit_console->append(" sent string ");
	ui_console->textEdit_console->append(data);
	ui_console->textEdit_console->append(" \n");
	return;
}

void Labonatip_console::emumeratePorts()
{
	QString fuilcellVendor = "16d0"; //TODO set the fluicell vendor

	ui_console->comboBox_serialInfo->clear();

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos) {
		QString s = QObject::tr("Port: ") + info.portName() + "\n"
			+ QObject::tr("Location: ") + info.systemLocation() + "\n"
			+ QObject::tr("Description: ") + info.description() + "\n"
			+ QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
			+ QObject::tr("Serial number: ") + info.serialNumber() + "\n"
			+ QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
			+ QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n";
			//+ QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

		ui_console->textEdit_console->append(s);
		ui_console->comboBox_serialInfo->addItem(info.portName());

		QString vendor = QString::number(info.vendorIdentifier(), 16);
		if (vendor == fuilcellVendor)
		{//TODO: this only attempt to match the vendor, is it also possible to check the product?
			ui_console->textEdit_console->append(" THIS SHOULD BE THE FUILCELL DEVICE \n\n");
		}
	} // TODO: in debug the detor crushes
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return;
}

void Labonatip_console::showPortInfo(int idx)
{
	if (idx == -1)
		return;

	QStringList list = ui_console->comboBox_serialInfo->itemData(idx).toStringList();
	ui_console->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
	ui_console->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
	ui_console->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
	ui_console->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
	ui_console->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
	ui_console->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void Labonatip_console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (localEchoEnabled)
			Labonatip_console::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

void Labonatip_console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Labonatip_console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Labonatip_console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

void Labonatip_console::readData()
{
	if (serial->isReadable()) {
		QByteArray data = serial->readAll();
//		cout << "data size " << data.size() << " content " << data.toStdString() << endl;
		ui_console->textEdit_console->append(data);
	}
}

void Labonatip_console::handleError(QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::ResourceError) {
		QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
		disconnectCOM();
	}
}

void Labonatip_console::exit()
{
    close();
    qApp->quit();
    delete ui_console;
}

Labonatip_console::~Labonatip_console()
{
  delete ui_console;
}
