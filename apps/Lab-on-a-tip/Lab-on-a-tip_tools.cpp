/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip_tools.h"


Labonatip_tools::Labonatip_tools(QWidget *parent ):
	QDialog (parent),
	m_comSettings(new COMSettings()),
	m_solutionNames(new solutionsNames()),
	m_pr_params(new pr_params()),
	ui_tools (new Ui::Labonatip_tools)
{
	ui_tools->setupUi(this );

	//load settings from file
	loadSettings("./settings/settings.ini");

	// initialize and connect serial port objects
	serial = new QSerialPort(parent);

	ui_tools->comboBox_serialInfo->clear();
	connect(ui_tools->comboBox_serialInfo,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Labonatip_tools::showPortInfo);

	// enumerate connected com ports
	enumerate();

	// connect GUI elements
	connect(ui_tools->pushButton_enumerate,
		SIGNAL(clicked()), this, SLOT(enumerate()));

	connect(ui_tools->pushButton_addMacroCommand,
		SIGNAL(clicked()), this, SLOT(addMacroCommand()));

	connect(ui_tools->pushButton_removeMacroCommand, 
		SIGNAL(clicked()), this, SLOT(removeMacroCommand()));

	connect(ui_tools->pushButton_becomeChild, 
		SIGNAL(clicked()), this, SLOT(becomeChild()));

	connect(ui_tools->pushButton_becomeParent, 
		SIGNAL(clicked()), this, SLOT(removeMacroCommand()));

	connect(ui_tools->pushButton_moveDown, 
		SIGNAL(clicked()), this, SLOT(moveDown()));

	connect(ui_tools->pushButton_moveUp, 
		SIGNAL(clicked()), this, SLOT(moveUp()));

	connect(ui_tools->pushButton_plusIndent, 
		SIGNAL(clicked()), this, SLOT(plusIndent()));

	connect(ui_tools->pushButton_saveMacro,
		SIGNAL(clicked()), this, SLOT(saveMacro()));

	connect(ui_tools->pushButton_loadMacro, 
		SIGNAL(clicked()), this, SLOT(loadMacro()));

	connect(ui_tools->pushButton_clearCommands, 
		SIGNAL(clicked()), this, SLOT(clearAllCommands()));

	connect(ui_tools->pushButton_duplicateLine, 
		SIGNAL(clicked()), this, SLOT(duplicateItem()));

	connect(ui_tools->treeWidget_macroTable, 
		SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, 
		SLOT(checkValidity(QTreeWidgetItem *, int)));

	connect(ui_tools->listWidget_options,
		SIGNAL(itemClicked(QListWidgetItem*)), this, 
		SLOT(onListMailItemClicked(QListWidgetItem*)));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Ok), 
		SIGNAL(clicked()), this, SLOT(okPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Discard), 
		SIGNAL(clicked()), this, SLOT(discardPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Apply), 
		SIGNAL(clicked()), this, SLOT(applyPressed()));

}

void Labonatip_tools::onListMailItemClicked(QListWidgetItem* _item)
{
	if (ui_tools->listWidget_options->item(0) == _item) {
		ui_tools->stackedWidget->setCurrentIndex(0);
	}
	if (ui_tools->listWidget_options->item(1) == _item) {
		ui_tools->stackedWidget->setCurrentIndex(1);
	}
	if (ui_tools->listWidget_options->item(2) == _item) {
		ui_tools->stackedWidget->setCurrentIndex(2);
	}
	if (ui_tools->listWidget_options->item(3) == _item) {
		ui_tools->stackedWidget->setCurrentIndex(3);
	}
	if (ui_tools->listWidget_options->item(4) == _item) {
		ui_tools->stackedWidget->setCurrentIndex(4);
	}

}

void Labonatip_tools::okPressed() {
	getCOMsettings();
	getSolutionSettings();
	addAllCommandsToMacro();

	saveSettings();

	//TODO other settings ! 
	emit ok();

	this->close();
}

void Labonatip_tools::discardPressed() {
	emit discard(); 
	
	this->close();
}

void Labonatip_tools::applyPressed() {
	getCOMsettings();
	getSolutionSettings();
	addAllCommandsToMacro(); 

	saveSettings();

	emit apply();

}

void Labonatip_tools::enumerate()
{
	ui_tools->comboBox_serialInfo->clear();
	// set serial port description for the first found port
	QString description;
	QString manufacturer;
	QString serialNumber;
	QSerialPortInfo serialInfo;
	QList<QSerialPortInfo> infos = serialInfo.availablePorts();
	for (const QSerialPortInfo &info : infos) {
		QStringList list;
		description = info.description();
		manufacturer = info.manufacturer();
		serialNumber = info.serialNumber();
		list << info.portName()
			<< (!description.isEmpty() ? description : "N/A")
			<< (!manufacturer.isEmpty() ? manufacturer : "N/A")
			<< (!serialNumber.isEmpty() ? serialNumber : "N/A")
			<< info.systemLocation()
			<< (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : "N/A")
			<< (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : "N/A");

		ui_tools->comboBox_serialInfo->addItem(list.first(), list);
	}
	/*	serialInfo.~QSerialPortInfo();
	cout << " infos size = " << infos.size() << endl;
	infos.clear();
	cout << " cleared = " << endl;
	infos.~QList();*/

}

void Labonatip_tools::addMacroCommand()
{
	QTreeWidgetItem *newItem = new QTreeWidgetItem;
	createNewCommand(*newItem);

	if (ui_tools->treeWidget_macroTable->topLevelItemCount() < 1 || 
		!ui_tools->treeWidget_macroTable->currentIndex().isValid()) {
		
		ui_tools->treeWidget_macroTable->insertTopLevelItem(0, newItem);
		return;
	}
	else {
		int row = ui_tools->treeWidget_macroTable->currentIndex().row();
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();
		
		if (ui_tools->treeWidget_macroTable->currentItem()->parent()) 
			parent->insertChild(row+1,newItem);
		else 
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row + 1, newItem);

		return;
	}

}

void Labonatip_tools::removeMacroCommand()
{
	if ( ui_tools->treeWidget_macroTable->currentItem() &&
		 ui_tools->treeWidget_macroTable->topLevelItemCount() > 0)  {// avoid crash is no elements in the table or no selection
		ui_tools->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
}

void Labonatip_tools::becomeChild()
{
	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection
	
	QTreeWidgetItem* item = ui_tools->treeWidget_macroTable->currentItem()->clone();
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();

	QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->itemAbove(ui_tools->treeWidget_macroTable->currentItem());
	if(parent){
		parent->addChild(item);
		parent->setExpanded(true);
		ui_tools->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
}

void Labonatip_tools::becomeParent()
{
	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection
	QTreeWidgetItem* item = ui_tools->treeWidget_macroTable->currentItem()->clone();
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();

/*	QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->itemAbove(ui_tools->treeWidget_macroTable->currentItem());
	if (parent) {
		parent->addChild(item);
		parent->setExpanded(true);
		ui_tools->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
*/
}

void Labonatip_tools::moveUp()
{
	QTreeWidgetItem *moveItem = ui_tools->treeWidget_macroTable->currentItem();
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();

	if (moveItem && row > 0 )
	{
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();
		if (parent) {
			parent->takeChild(row);
			parent->insertChild(row - 1, moveItem);
		}
		else {
			ui_tools->treeWidget_macroTable->takeTopLevelItem(row);
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row - 1, moveItem);
		}
		ui_tools->treeWidget_macroTable->setCurrentItem(moveItem);
	}
}

void Labonatip_tools::moveDown()
{
	QTreeWidgetItem *moveItem = ui_tools->treeWidget_macroTable->currentItem();
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();
	int number_of_items = ui_tools->treeWidget_macroTable->topLevelItemCount();

	if (moveItem && row >= 0 && row < number_of_items - 1)
	{
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();

		if (parent) {
			int childCount = parent->childCount();
			if (row >= childCount - 1 ) return; // stops at the last

			parent->takeChild(row);
			parent->insertChild(row + 1, moveItem);
		}
		else {
			ui_tools->treeWidget_macroTable->takeTopLevelItem(row);
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row + 1, moveItem);
		}
		ui_tools->treeWidget_macroTable->setCurrentItem(moveItem);
	}
}

void Labonatip_tools::plusIndent()
{
	QTreeWidgetItem *newItem = new QTreeWidgetItem;
	createNewCommand(*newItem);

	if (!ui_tools->treeWidget_macroTable->currentIndex().isValid()) {
		ui_tools->treeWidget_macroTable->insertTopLevelItem(0, newItem);
		return;
	}
	else {
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem();
		parent->insertChild(0, newItem);
		return;
	}

}

bool Labonatip_tools::checkValidity(QTreeWidgetItem *_item, int _column)
{
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();
	cout << "check validity on row " << row << " and column " << _column << endl;

	switch (_column) {
	case 0:
		// check name
		// every thing is fine
		cout << "case : " << _column << endl;
		break;
	case 1: {
		// check loops
		bool isNumeric;
		cout << "case : " << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Loop is not a valid number, \n value must be a positive integer number!");
			_item->setText(_column, QString("1"));
			return false;
		}
		break;
	}
	case 2: {
		// check pon
		bool isNumeric; 
		cout << "case : "  << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Pon is not a valid number, \n value must be a positive integer number in [0, 350]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		if (number < 0 || number > 500) {
			QMessageBox::warning(this, "Warning !", " Pon is out of range, \n value must be a positive integer number in [0, 350]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		break;
	}
	case 3: {
		// check poff
		bool isNumeric;
		cout << "case : " << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Poff is not a valid number, \n value must be a positive integer number in [0, 500]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		if (number < 0 || number > 500) {
			QMessageBox::warning(this, "Warning !", " Poff is out of range, \n value must be a positive integer number in [0, 500]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		break;
	}
	case 4:{
		// check v_s
		bool isNumeric;
		cout << "case : " << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " V_switch is not a valid number, \n value must be a positive integer number in [-350, 0]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		if (number < -350 || number > 0) {
			QMessageBox::warning(this, "Warning !", " V_switch is out of range, \n value must be a positive integer number in [-350, 0]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		break;
	}
	case 5: {
		// check v_r
		bool isNumeric;
		cout << "case : " << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " V_recirc is not a valid number, \n value must be a positive integer number in [0, 350]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		if (number < -350 || number > 0) {
			QMessageBox::warning(this, "Warning !", " V_recirc is out of range, \n value must be a positive integer number in [0, 350]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		break;
	}
	case 6: {
		// check duration
		bool isNumeric;
		cout << "case : " << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric); // should this be an integer?
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Duration is not a valid number, \n value must be a positive integer number !");
			_item->setText(_column, QString("0"));
			return false;
		}
		if (number <= 0 ) {
			QMessageBox::warning(this, "Warning !", " Duration is out of range, \n value must be a positive integer number !");
			_item->setText(_column, QString("0"));
			return false;
		}
		break;
	}
	case 7: {
		// check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
		bool isNumeric;
		cout << "case : " << _column << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Open valsve is not a valid number, \n value must be a positive integer number in [0, 4]! \n where 0 = no open valve, 1,2,3,4 open valves 1,2,3,4. Only one valve at time can be open");
			_item->setText(_column, QString("0"));
			return false;
		}
		if (number < 0 || number > 4) {
			QMessageBox::warning(this, "Warning !", " Duration is out of range, \n value must be a positive integer number in [0, 350]!");
			_item->setText(_column, QString("0"));
			return false;
		}
		break;
	}
	case 8:{
		// check
		cout << "case : " << _column << endl;
		break;
	}
	case 9:{
		// check
		cout << "case : " << _column << endl;
		break;
	}
	default:{
		// default function active if none of the previous
		cout << "default : " << _column << endl;
		break;
	}
	}

	return false;
}

void Labonatip_tools::duplicateItem()
{
	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection

	QTreeWidgetItem *newItem = ui_tools->treeWidget_macroTable->currentItem()->clone();


	if (ui_tools->treeWidget_macroTable->topLevelItemCount() < 1 ||
		!ui_tools->treeWidget_macroTable->currentIndex().isValid()) {

		ui_tools->treeWidget_macroTable->insertTopLevelItem(0, newItem);

		return;
	}
	else {
		int row = ui_tools->treeWidget_macroTable->currentIndex().row();
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();

		if (parent) {
			parent->insertChild(row + 1, newItem);
		}
		else {
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row + 1, newItem);
		}
		return;
	}

}

void Labonatip_tools::visualizeItemProperties()
{
	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection

	QTreeWidgetItem *moveItem = ui_tools->treeWidget_macroTable->currentItem();
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();
	int column = ui_tools->treeWidget_macroTable->currentIndex().column();
	int childCount = ui_tools->treeWidget_macroTable->currentItem()->childCount();
	int indexOfChild = -1;
	int depth = -1;
	bool hasParent = false;
	QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();
	if (parent) {
		hasParent = true;
		indexOfChild = ui_tools->treeWidget_macroTable->currentItem()->indexOfChild(
			ui_tools->treeWidget_macroTable->currentItem() );
		depth = 1;

		while (parent != 0) {
			depth++;
			parent = parent->parent();
		}

	}

	int number_of_items = ui_tools->treeWidget_macroTable->topLevelItemCount();

	cout << "\n  row" << row  
		 << "\n  column  " << column
		 << "\n  childCount  " << childCount
		 << "\n  hasParent  " << hasParent
		 << "\n  indexOfChild  " << indexOfChild
		 << "\n  depth  " << depth 
		 << "\n  number_of_items  " << number_of_items
		 << endl;
}

void Labonatip_tools::createNewCommand(QTreeWidgetItem & _command)
{
	_command.setText(0, "Name"); // name
	_command.setText(1, "1"); // loop
	_command.setText(2, "190"); // pon
	_command.setText(3, "21"); // poff
	_command.setText(4, "-115"); // v_s
	_command.setText(5, "-115"); // v_r
	_command.setText(6, "60"); // duration
	_command.setText(7, "0"); // open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4
	_command.setCheckState(8, Qt::CheckState::Unchecked); // wait sync
	_command.setCheckState(9, Qt::CheckState::Unchecked); // sync out
	_command.setCheckState(10, Qt::CheckState::Unchecked); // ask
	_command.setText(10, "questions?"); // ask message 
	_command.setText(11, "go science !!!"); // status message
	_command.setFlags(_command.flags() | (Qt::ItemIsEditable));
}


void Labonatip_tools::showPortInfo(int idx)
{
	if (idx == -1)
		return;

	QStringList list = ui_tools->comboBox_serialInfo->itemData(idx).toStringList();
	ui_tools->descriptionLabel->setText(
		tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr("N/A")));
	ui_tools->manufacturerLabel->setText(
		tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr("N/A")));
	ui_tools->serialNumberLabel->setText(
		tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr("N/A")));
	ui_tools->locationLabel->setText(
		tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr("N/A")));
	ui_tools->vidLabel->setText(
		tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr("N/A")));
	ui_tools->pidLabel->setText(
		tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr("N/A")));
}

void Labonatip_tools::getCOMsettings()
{
	m_comSettings->name = ui_tools->comboBox_serialInfo->currentText();
	m_comSettings->baudRate = ui_tools->comboBox_baudRate->currentText().toInt();
	//m_comSettings->dataBits = 
	//m_comSettings->flowControl =
	//m_comSettings->parity =
	//m_comSettings->stopBits =

	//TODO other settings ! 
}

void Labonatip_tools::getSolutionSettings()
{
	m_solutionNames->sol1 = ui_tools->lineEdit_sol1_name->text();
	m_solutionNames->sol2 = ui_tools->lineEdit_sol2_name->text();
	m_solutionNames->sol3 = ui_tools->lineEdit_sol3_name->text();
	m_solutionNames->sol4 = ui_tools->lineEdit_sol4_name->text();
	//TODO other settings ! 
}


void Labonatip_tools::loadSettings(QString _path)
{

	if (_path.isEmpty())
	{
		cout << "Warning !  ::  Setting file not found !\n please set the setting file " << endl;
		//_path = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		//	"Ini file (*.ini);; Data file (*.dat);; Binary File (*.bin);; All Files(*.*)", 0);
	}

	cout << "the path is : " << _path.toStdString() << endl;

	m_settings = new QSettings(_path, QSettings::IniFormat);

	// read default group
	QString user = m_settings->value("default/user", "No owner found").toString();
	ui_tools->lineEdit_userName->setText(user);

	int year = m_settings->value("default/year", "2017").toInt();
	ui_tools->lineEdit_year->setText(QString::number(year));

	QString month = m_settings->value("default/month", "No owner found").toString();
	ui_tools->lineEdit_month->setText(month);

	// read com group
	//ComName
	QString comPort = m_settings->value("COM/ComName", "COM1").toString();
	m_comSettings->name = comPort;

	//BaudRate
	int baudRate = m_settings->value("COM/BaudRate", "115200").toInt();
	m_comSettings->baudRate = static_cast<qint32>(baudRate);
	ui_tools->comboBox_baudRate->setCurrentIndex(7);  // baudrate forced value 115200 

	//DataBits
	int dataBits = m_settings->value("COM/DataBits", "8").toInt();
	switch (dataBits) {
	case 5:
		m_comSettings->dataBits = QSerialPort::DataBits::Data5;
		break;
	case 6:
		m_comSettings->dataBits = QSerialPort::DataBits::Data6;
		break;
	case 7:
		m_comSettings->dataBits = QSerialPort::DataBits::Data7;
		break;
	case 8:
		m_comSettings->dataBits = QSerialPort::DataBits::Data8;
		break;
	default:
		cout << " Error data bit cannot be read, using default value 8" << endl;
		m_comSettings->dataBits = QSerialPort::DataBits::Data8;
		break;
	}

	//Parity = NoParity
	QString parity = m_settings->value("COM/Parity", "NoParity").toString();
	m_comSettings->parity = QSerialPort::Parity::NoParity; //TODO: no intepretation yet

	//StopBits = 1
	int stopBits = m_settings->value("COM/StopBits", "1").toInt();
	m_comSettings->stopBits = QSerialPort::StopBits::OneStop; //TODO: no intepretation yet

	//FlowControl = noFlow
	QString flowControl = m_settings->value("COM/FlowControl", "noFlow").toString();
	m_comSettings->flowControl = QSerialPort::FlowControl::NoFlowControl; //TODO: no intepretation yet

	
	// read pr_limits group
	int p_on_max = m_settings->value("pr_limits/p_on_max", "500").toInt();
	ui_tools->lineEdit_p_on_max->setText(QString::number(p_on_max));
	m_pr_params->p_on_max = p_on_max;

	int p_on_min = m_settings->value("pr_limits/p_on_min", "0").toInt();
	ui_tools->lineEdit_p_on_min->setText(QString::number(p_on_min));
	m_pr_params->p_on_min = p_on_min;

	int p_on_default = m_settings->value("pr_limits/p_on_default", "0").toInt();
	ui_tools->lineEdit_p_on_default->setText(QString::number(p_on_default));
	m_pr_params->p_on_default = p_on_default;

	int p_off_max = m_settings->value("pr_limits/p_off_max", "500").toInt();
	ui_tools->lineEdit_p_off_max->setText(QString::number(p_off_max));
	m_pr_params->p_off_max = p_off_max;

	int p_off_min = m_settings->value("pr_limits/p_off_min", "0").toInt();
	ui_tools->lineEdit_p_off_min->setText(QString::number(p_off_min));
	m_pr_params->p_off_min = p_off_min;

	int p_off_default = m_settings->value("pr_limits/p_off_default", "0").toInt();
	ui_tools->lineEdit_p_off_default->setText(QString::number(p_off_default));
	m_pr_params->p_off_default = p_off_default;

	int v_switch_max = m_settings->value("pr_limits/v_switch_max", "0").toInt();
	ui_tools->lineEdit_v_switch_max->setText(QString::number(v_switch_max));
	m_pr_params->v_switch_max = v_switch_max;

	int v_switch_min = m_settings->value("pr_limits/v_switch_min", "-350").toInt();
	ui_tools->lineEdit_v_switch_min->setText(QString::number(v_switch_min));
	m_pr_params->v_switch_min = v_switch_min;

	int v_switch_default = m_settings->value("pr_limits/v_switch_default", "-350").toInt();
	ui_tools->lineEdit_v_switch_default->setText(QString::number(v_switch_default));
	m_pr_params->v_switch_default = v_switch_default;

	int v_recirc_max = m_settings->value("pr_limits/v_recirc_max", "0").toInt();
	ui_tools->lineEdit_v_recirc_max->setText(QString::number(v_recirc_max));
	m_pr_params->v_recirc_max = v_recirc_max;

	int v_recirc_min = m_settings->value("pr_limits/v_recirc_min", "-350").toInt();
	ui_tools->lineEdit_v_recirc_min->setText(QString::number(v_recirc_min));
	m_pr_params->v_recirc_min = v_recirc_min;

	int v_recirc_default = m_settings->value("pr_limits/v_recirc_default", "-350").toInt();
	ui_tools->lineEdit_v_recirc_default->setText(QString::number(v_recirc_default));
	m_pr_params->v_recirc_default = v_recirc_default;

	// read server group


	//Read solution names block
	QString solname1 = m_settings->value("solutionNames/solution1", "no name").toString();
	ui_tools->lineEdit_sol1_name->setText(solname1);
	m_solutionNames->sol1 = solname1; //TODO: no intepretation yet

	QString solname2 = m_settings->value("solutionNames/solution2", "no name").toString();
	ui_tools->lineEdit_sol2_name->setText(solname2);
	m_solutionNames->sol2 = solname2; //TODO: no intepretation yet

	QString solname3 = m_settings->value("solutionNames/solution3", "no name").toString();
	ui_tools->lineEdit_sol3_name->setText(solname3);
	m_solutionNames->sol3 = solname3; //TODO: no intepretation yet

	QString solname4 = m_settings->value("solutionNames/solution4", "no name").toString();
	ui_tools->lineEdit_sol4_name->setText(solname4);
	m_solutionNames->sol4 = solname4; //TODO: no intepretation yet

									  // read Leap group
	bool myParam1 = m_settings->value("MyParam/myParam1", "true").toBool();
	double myParam2 = m_settings->value("MyParam/myParam2", "40.0").toDouble();

	m_settings->setValue("MyParam/myParam3", "10.2");
	m_settings->sync();

	// TODO set the com parameters
	//comSettings->baudRate = BaudRate;
	//comSettings->name = ComPort;


	cout << " Load settings: \n"
		<< "    default/owner " << user.toStdString() << " \n"
		<< "    default/date " << year << " \n"
		<< "    default/month " << month.toStdString() << " \n\n"
		<< "    COM/ComPort " << comPort.toStdString() << " \n"
		<< "    COM/BaudRate " << baudRate << " \n"
		<< "    COM/DataBits " << dataBits << " \n"
		<< "    COM/Parity " << parity.toStdString() << " \n"
		<< "    COM/StopBits " << stopBits << " \n"
		<< "    COM/FlowControl " << flowControl.toStdString() << " \n"
		<< "    MyParam/myParam1 " << myParam1 << " \n"
		<< "    MyParam/myParam2 " << myParam2 << " \n\n"

		<< endl;

}

void Labonatip_tools::saveSettings()
{


	// [default]
	// user = 
	m_settings->setValue("default/user", ui_tools->lineEdit_userName->text());
	// year = 
	m_settings->setValue("default/user", ui_tools->lineEdit_year->text());
	// month = 
	m_settings->setValue("default/user", ui_tools->lineEdit_month->text());

	// [COM]
	// ComName = COM_
	m_settings->setValue("COM/ComName", m_comSettings->name);
	// BaudRate = 115200
	m_settings->setValue("COM/BaudRate", ui_tools->comboBox_baudRate->currentText());
	// DataBits = 8
	m_settings->setValue("COM/DataBits", ui_tools->comboBox_dataBit->currentText());
	// Parity = NoParity
	m_settings->setValue("COM/Parity", ui_tools->comboBox_parity->currentText());
	// StopBits = 1
	m_settings->setValue("COM/StopBits", ui_tools->comboBox_stopBit->currentText());
	// FlowControl = noFlow
	m_settings->setValue("COM/FlowControl", ui_tools->comboBox_flowControl->currentText());

	// [pr_limits]
	// p_on_max = 
	m_settings->setValue("pr_limits/p_on_max", ui_tools->lineEdit_p_on_max->text());
	// p_on_min =
	m_settings->setValue("pr_limits/p_on_min", ui_tools->lineEdit_p_on_min->text());
	// p_on_default = 
	m_settings->setValue("pr_limits/p_on_default", ui_tools->lineEdit_p_on_default->text());
	// p_off_max = 
	m_settings->setValue("pr_limits/p_off_max", ui_tools->lineEdit_p_off_max->text());
	// p_off_min = 
	m_settings->setValue("pr_limits/p_off_min", ui_tools->lineEdit_p_off_min->text());
	// p_off_default = 
	m_settings->setValue("pr_limits/p_off_default", ui_tools->lineEdit_p_off_default->text());
	// v_switch_max = 
	m_settings->setValue("pr_limits/v_switch_max", ui_tools->lineEdit_v_switch_max->text());
	// v_switch_min = 
	m_settings->setValue("pr_limits/v_switch_min", ui_tools->lineEdit_v_switch_min->text());
	// v_switch_default = 
	m_settings->setValue("pr_limits/v_switch_default", ui_tools->lineEdit_v_switch_default->text());
	// v_recirc_max = 
	m_settings->setValue("pr_limits/v_recirc_max", ui_tools->lineEdit_v_recirc_max->text());
	// v_recirc_min = 
	m_settings->setValue("pr_limits/v_recirc_min", ui_tools->lineEdit_v_recirc_min->text());
	// v_recirc_default = 
	m_settings->setValue("pr_limits/v_recirc_default", ui_tools->lineEdit_v_recirc_default->text());

	// [solutionNames]
	// solution1 = CuSO4
	m_settings->setValue("solutionNames/solution1", ui_tools->lineEdit_sol1_name->text());
	// solution2 = NaCl
	m_settings->setValue("solutionNames/solution2", ui_tools->lineEdit_sol2_name->text());
	// solution3 = NaHCO3
	m_settings->setValue("solutionNames/solution3", ui_tools->lineEdit_sol3_name->text());
	// solution4 = FeS
	m_settings->setValue("solutionNames/solution4", ui_tools->lineEdit_sol4_name->text());


	m_settings->sync();
	//TODO !!!!

}

void Labonatip_tools::addAllCommandsToMacro()
{

	std::vector<QTreeWidgetItem*> items;
	m_macro->clear();

	for (int i = 0; i < ui_tools->treeWidget_macroTable->topLevelItemCount(); ++i) {

		QTreeWidgetItem *item = ui_tools->treeWidget_macroTable->topLevelItem(i);

		if (item->childCount() < 0) { // if no children, just add the line 
			items.push_back(ui_tools->treeWidget_macroTable->topLevelItem(i));
		}
		else
		{// otherwise we need to traverse the tree
			items.push_back(ui_tools->treeWidget_macroTable->topLevelItem(i));
			for (int loop = 0; loop < item->text(1).toInt(); loop++) {
				// we need to check how many times we need to run the operations
				// and add the widget to the list
				for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
					items.push_back(item->child(childrenCount));
				}

			}

		}

	}

	for (int i = 0; i < items.size(); ++i)
	{
		fluicell::PPC1api::command new_command;
		new_command.P_on = items.at(i)->text(2).toInt();
		new_command.P_off = items.at(i)->text(3).toInt();
		new_command.V_switch = items.at(i)->text(4).toInt();
		new_command.V_recirc = items.at(i)->text(5).toInt();
		new_command.Duration = items.at(i)->text(6).toInt();

		int valve = items.at(i)->text(7).toInt();
		switch (valve) {  // intereptation of the valve value
		case 0:
			new_command.open_valve_a = 0;
			new_command.open_valve_b = 0;
			new_command.open_valve_c = 0;
			new_command.open_valve_d = 0;
			break;
		case 1:
			new_command.open_valve_a = 1;
			new_command.open_valve_b = 0;
			new_command.open_valve_c = 0;
			new_command.open_valve_d = 0;
			break;
		case 2:
			new_command.open_valve_a = 0;
			new_command.open_valve_b = 1;
			new_command.open_valve_c = 0;
			new_command.open_valve_d = 0;
			break;
		case 3:
			new_command.open_valve_a = 0;
			new_command.open_valve_b = 0;
			new_command.open_valve_c = 1;
			new_command.open_valve_d = 0;
			break;
		case 4:
			new_command.open_valve_a = 0;
			new_command.open_valve_b = 0;
			new_command.open_valve_c = 0;
			new_command.open_valve_d = 1;
			break;
		default:
			// Code
			break;
		}

		new_command.wait_sync = items.at(i)->checkState(8);
		new_command.sync_out = items.at(i)->checkState(9);
		new_command.ask = items.at(i)->checkState(10);
		new_command.ask_message = items.at(i)->text(10).toStdString();
		new_command.status_message = items.at(i)->text(11).toStdString();


		m_macro->push_back(new_command);
	}


	cout << " the current macro will run " << m_macro->size() << " commands " << endl;



}


bool Labonatip_tools::loadMacro()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QString fine_name = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);

	QFile macroFile(fine_name);
	if (macroFile.exists() && macroFile.open(QIODevice::ReadWrite))
	{
		QByteArray content = macroFile.readLine();

		QList<QTreeWidgetItem *> *list = new QList<QTreeWidgetItem *>();
		while (!content.isEmpty())
		{
			QTreeWidgetItem *newItem = new QTreeWidgetItem();
			QTreeWidgetItem *parent = new QTreeWidgetItem();

			if (decodeMacroCommand(content, *newItem)) {
				if (getLevel(*newItem) == 0) // we are at top level
				{
					ui_tools->treeWidget_macroTable->addTopLevelItem(newItem);
					//list->push_back(newItem);
					cout << " top level item " << newItem->text(0).toStdString()
						<< " level " << newItem->text(13).toStdString() << endl;
					parent = newItem->clone();
				}
				if (getLevel(*newItem) > 0)  // we are at the first level
				{
					//getLastNode(ui_tools->treeWidget_macroTable, *parent);
					cout << " item level " << getLevel(*newItem) << " text " << newItem->text(0).toStdString() << endl;
					QTreeWidgetItem *item = new QTreeWidgetItem();
					QTreeWidgetItemIterator *item_iterator =
						new QTreeWidgetItemIterator(ui_tools->treeWidget_macroTable, QTreeWidgetItemIterator::All);
					QTreeWidgetItemIterator it(ui_tools->treeWidget_macroTable);
					while (*it) { // this will just get the last node
						item = (*it);
						++it;
					}
					if (getLevel(*item) == getLevel(*newItem)) {
						item->parent()->addChild(newItem);
					}
					if (getLevel(*newItem) == getLevel(*item) + 1) {
						item->addChild(newItem);
					}
					if (getLevel(*newItem) < getLevel(*item)) {
						QTreeWidgetItem *parentItem = item;
						for (int i = 0; i < getLevel(*item) - getLevel(*newItem); i++) {
							parentItem = parentItem->parent();
						}
						parentItem->parent()->addChild(newItem);
					}
					ui_tools->treeWidget_macroTable->update();
				}
				else { // there is something wrong !! 
					   //QMessageBox::warning(this, "Warning !", "Negative level, file corrupted  ! ");
				}
			}
			content = macroFile.readLine();
		}
	
		//resize columns to content
		for (int i = 0; i < ui_tools->treeWidget_macroTable->columnCount(); i++) {
			ui_tools->treeWidget_macroTable->resizeColumnToContents(i);
		}
		//ui_tools->treeWidget_macroTable->addTopLevelItems(*list);

	}
	else {
		QMessageBox::warning(this, "Warning !", "File not found ! <br>" + fine_name);
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		return false;

	}


	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

											  // do something with the new open file

	return true;
}


bool Labonatip_tools::loadMacro(const QString _file_name)
{

	return false;
}

bool Labonatip_tools::saveMacro()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save something"), QDir::currentPath(),  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);

	if (!saveMacro(fileName)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "File not saved ! <br>" + fileName);
		return false;
	}
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

	return true;
}



bool Labonatip_tools::saveMacro(QString _file_name)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	if (_file_name.isEmpty()) {
		_file_name = QFileDialog::getSaveFileName(this, tr("Save something"), QDir::currentPath(),  // dialog to open files
			"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);
		//TODO check file name validity
	}

	if (!_file_name.endsWith(".macro", Qt::CaseSensitive)) {
		_file_name.append(".macro");
	}

	QFile macroFile(_file_name);
	if (macroFile.open(QIODevice::ReadWrite))
	{
		QTextStream stream(&macroFile);
		QString header = createHeader();
		stream << header << endl;

		QList<QStringList> result = visitTree(ui_tools->treeWidget_macroTable);

		cout << " result size " << result.size() << endl;

		for (int i = 0; i < result.size(); i++) {
			for (int j = 0; j < result.at(i).size(); j++)
			{
				//cout << " element " << i << " is " << result.at(i).at(j).toStdString() << endl;
				stream << result.at(i).at(j) << "#";
			}
			stream << "§" << endl;
		}
	}
	else {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "File not saved ! <br>" + _file_name);
		return false;
	}


	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return true;
}


void Labonatip_tools::visitTree(QTreeWidgetItem *_item) {


}

int Labonatip_tools::getLevel(QTreeWidgetItem _item)
{
	int level;

	// for our item structure the level is on the column number 12
	bool success = false;
	level = _item.text(12).toInt(&success);
	if (success) return level;
	else return -1;

}


void Labonatip_tools::getLastNode(QTreeWidget *_tree, QTreeWidgetItem *_item)
{
	QTreeWidgetItemIterator *item_iterator =
		new QTreeWidgetItemIterator(_tree, QTreeWidgetItemIterator::All);
	QTreeWidgetItemIterator it(_tree);
	while (*it) {
		_item = (*it);
		++it;
		//		cout << "getLastNode first column " << _item.text(0).toStdString() << endl;
	}
}

void Labonatip_tools::visitTree(QList<QStringList> &_list, QTreeWidgetItem *_item) {

	QStringList _string_list;

	int depth = 0;
	QTreeWidgetItem *parent = _item->parent();
	while (parent != 0) {
		depth++;
		parent = parent->parent();
	}

	//there is a problem with the size of the column

	for (int i = 0; i < 12; i++)//_item->columnCount(); i++)
	{
		if (i == 8 || i == 9)
		{
			_string_list.push_back(QString::number(_item->checkState(i)));
		}
		if (i == 10)
		{
			_string_list.push_back(QString::number(_item->checkState(i)));
			_string_list.push_back(_item->text(i));
		}
		else
		{
			_string_list.push_back(_item->text(i));

		}
	}

	_string_list.push_back(QString::number(depth)); // push the depth of the command as last

	_list.push_back(_string_list);

	for (int i = 0; i<_item->childCount(); ++i)
		visitTree(_list, _item->child(i));
}

QList<QStringList> Labonatip_tools::visitTree(QTreeWidget *_tree) {
	QList<QStringList> list;
	for (int i = 0; i<_tree->topLevelItemCount(); ++i)
		visitTree(list, _tree->topLevelItem(i));
	return list;
}

bool Labonatip_tools::decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item)
{
	QStringList data_string;
	if (_command.at(0) == *"%") {
		// do nothing, just discard the line
		//	cout << " this line belogs to the header" << endl;
	}
	else
	{
		//		cout << " this line does not belog to the header " << _command.toStdString() << "  size " << _command.size() << endl;

		for (int i = 0; i < _command.size() - 2; i++)  // the endline has 2 end characters, # for the string and § for the line
		{
			QString data = "";
			while (_command.at(i) != *"#")
			{
				data.append(_command.at(i));
				i++;
				if (_command.at(i) == *"§") break; // endline
			}
			data_string.push_back(data);
		}
	}

	if (data_string.size() < 14) {
		return false;  // something went wrong
	}


	// fill the qtreewidget item
	_out_item.setText(0, data_string.at(0)); // name
	_out_item.setText(1, data_string.at(1)); // loop
	_out_item.setText(2, data_string.at(2)); // pon
	_out_item.setText(3, data_string.at(3)); // poff
	_out_item.setText(4, data_string.at(4)); // v_s
	_out_item.setText(5, data_string.at(5)); // v_r
	_out_item.setText(6, data_string.at(6)); // duration
	_out_item.setText(7, data_string.at(7)); // open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4

	if (data_string.at(8) == "2")
		_out_item.setCheckState(8, Qt::CheckState::Checked); // wait sync
	else _out_item.setCheckState(8, Qt::CheckState::Unchecked); // wait sync
	_out_item.setText(8, data_string.at(9)); // wait sync message 

	if (data_string.at(10) == "2")
		_out_item.setCheckState(9, Qt::CheckState::Checked); // sync out
	else _out_item.setCheckState(9, Qt::CheckState::Unchecked); // sync out
	_out_item.setText(9, data_string.at(11)); // sync out message 

	if (data_string.at(12) == "2")
		_out_item.setCheckState(10, Qt::CheckState::Checked); // sync out
	else _out_item.setCheckState(10, Qt::CheckState::Unchecked); // sync out
	_out_item.setText(10, data_string.at(13)); // ask message 

	_out_item.setText(11, data_string.at(14)); // status message
	_out_item.setText(12, data_string.at(15)); // status message
	_out_item.setFlags(_out_item.flags() | (Qt::ItemIsEditable));

	return true;
}




QString Labonatip_tools::createHeader()
{

	QString header;
	header.append(tr("%% +---------------------------------------------------------------------------+\n"));
	header.append(tr("%% |                           FLUICELL LAB-ON-A-TIP                           |\n"));
	header.append(tr("%% |                                                                           |\n"));
	header.append(tr("%% |                                                                           |\n"));
	header.append(tr("%% |                          http://www.fluicell.com/                         |\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+\n"));
	header.append(tr("%% \n"));
	header.append(tr("%% Macro Header V. 0.1 \n"));
	header.append(tr("%% file created on dd/mm/yyyy \n"));
	header.append(tr("%% Fluicell Lab-on-a-tip macro file description \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% List of parameters: \n"));
	header.append(tr("%%  - name \n"));
	header.append(tr("%%  - loops \n"));
	header.append(tr("%%  - P_on (mbar) \n"));
	header.append(tr("%%  - P_off (mbar) \n"));
	header.append(tr("%%  - V_switch (mbar) \n"));
	header.append(tr("%%  - V_recirc (mbar) \n"));
	header.append(tr("%%  - Duration (sec) \n"));
	header.append(tr("%%  - OpenSol1 (int) \n"));
	header.append(tr("%%  - waitSync (bool) \n"));
	header.append(tr("%%  - syncOut (bool) \n"));
	header.append(tr("%%  - ask (bool) \n"));
	header.append(tr("%%  - ask_message (bool) \n"));
	header.append(tr("%%  - status_message (string) \n"));
	header.append(tr("%%  - level \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% name loops P_on P_off  V_switch  V_recirc  Duration  OpenSol  waitSync  syncOut  ask ask_message status_message level\n%"));

	return header;
}

void Labonatip_tools::clearAllCommands() {
	ui_tools->treeWidget_macroTable->clear();
}

Labonatip_tools::~Labonatip_tools() {
  delete ui_tools;
}