/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "tools.h"
#include  <QCheckBox>

Labonatip_tools::Labonatip_tools(QWidget *parent ):
	QMainWindow(parent),
	ui_tools(new Ui::Labonatip_tools), 
	m_comSettings(new COMSettings()),
	m_solutionParams(new solutionsParams()),
	m_pr_params(new pr_params()),
	m_GUI_params(new GUIparams()),
	m_setting_file_name("./settings/settings.ini")
{
	ui_tools->setupUi(this );

	//load settings from file
	loadSettings(m_setting_file_name);

	m_str_warning.append(tr("Warning"));
	m_str_factory_reset.append(tr("This will reset used defined settings and parameters to the factory default value. \nAre you sure ?"));

	//make sure to start from the initial page
	ui_tools->actionGeneral->setChecked(true);
	ui_tools->stackedWidget->setCurrentIndex(0);

	ui_tools->comboBox_serialInfo->clear();
	connect(ui_tools->comboBox_serialInfo,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Labonatip_tools::showPortInfo);

	connect(ui_tools->comboBox_language,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Labonatip_tools::languageChanged);


	connect(ui_tools->comboBox_toolButtonStyle,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Labonatip_tools::toolButtonStyleChanged);

	connect(ui_tools->checkBox_enableToolTips,
		SIGNAL(stateChanged(int)), this, SLOT(enableToolTip(int))); //TODO:

	// enumerate connected com ports
	enumerate();

	// connect GUI elements: communication tab
	connect(ui_tools->pushButton_enumerate,
		SIGNAL(clicked()), this, SLOT(enumerate()));

	connect(ui_tools->actionGeneral,
		SIGNAL(triggered()), this,
		SLOT(goToPage1()));

	connect(ui_tools->actionSolution,
		SIGNAL(triggered()), this,
		SLOT(goToPage2()));

	connect(ui_tools->actionPressure,
		SIGNAL(triggered()), this,
		SLOT(goToPage3()));

	connect(ui_tools->actionCommunication,
		SIGNAL(triggered()), this,
		SLOT(goToPage4()));

	// connect color solution settings
	connect(ui_tools->horizontalSlider_colorSol1,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol1Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol2,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol2Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol3,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol3Changed(int)));

	connect(ui_tools->horizontalSlider_colorSol4,
		SIGNAL(valueChanged(int)), this,
		SLOT(colorSol4Changed(int)));

	connect(ui_tools->pushButton_emptyWaste,
		SIGNAL(clicked()), this, SLOT(emptyWastePressed()));

	
	connect(ui_tools->pushButton_refillSolution,
		SIGNAL(clicked()), this, SLOT(refillSolutionPressed()));

	connect(ui_tools->checkBox_disableTimer,
		SIGNAL(stateChanged(int)), this,
		SLOT(setContinuousFow(int))); 

	connect(ui_tools->pushButton_toDefault,
		SIGNAL(clicked()), this, SLOT(resetToDefaultValues()));

	// connect tool window events Ok, Cancel, Apply
	connect(ui_tools->buttonBox->button(QDialogButtonBox::Ok), 
		SIGNAL(clicked()), this, SLOT(okPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Cancel), 
		SIGNAL(clicked()), this, SLOT(cancelPressed()));

	connect(ui_tools->buttonBox->button(QDialogButtonBox::Apply), 
		SIGNAL(clicked()), this, SLOT(applyPressed()));

}

void Labonatip_tools::goToPage1()
{
	ui_tools->actionGeneral->setChecked(true);
	ui_tools->actionSolution->setChecked(false);
	ui_tools->actionPressure->setChecked(false);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(0);

}
void Labonatip_tools::goToPage2()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionSolution->setChecked(true);
	ui_tools->actionPressure->setChecked(false);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(1);
}
void Labonatip_tools::goToPage3()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionSolution->setChecked(false);
	ui_tools->actionPressure->setChecked(true);
	ui_tools->actionCommunication->setChecked(false);
	ui_tools->stackedWidget->setCurrentIndex(2);
}
void Labonatip_tools::goToPage4()
{
	ui_tools->actionGeneral->setChecked(false);
	ui_tools->actionSolution->setChecked(false);
	ui_tools->actionPressure->setChecked(false);
	ui_tools->actionCommunication->setChecked(true);
	ui_tools->stackedWidget->setCurrentIndex(3);
}

void Labonatip_tools::okPressed() {

	getCOMsettingsFromGUI();
	getSolutionSettingsFromGUI();
	getGUIsettingsFromGUI();
	getPRsettingsFromGUI();

	//TODO manual save for now
	//saveSettings();

	emit ok();
	this->close();
}

void Labonatip_tools::cancelPressed() {

	emit cancel();
	this->close();
}


void Labonatip_tools::applyPressed() {

	getCOMsettingsFromGUI();
	getSolutionSettingsFromGUI();
	getGUIsettingsFromGUI();
	getPRsettingsFromGUI();
	//TODO manual save for now
	//saveSettings();

	emit apply();
}

void Labonatip_tools::refillSolutionPressed() {
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::refillSolutionPressed " << endl;
	emit refillSolution();

}

void Labonatip_tools::emptyWastePressed() {
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::emptyWellsPressed " << endl;
	emit emptyWaste();

}


void Labonatip_tools::enumerate()
{
	ui_tools->comboBox_serialInfo->clear();
	// set serial port description for the first found port
	QString description;
	QString manufacturer;
	QString serialNumber;

	// try to get device information
	std::vector<serial::PortInfo> devices = serial::list_ports();
	std::vector<fluicell::PPC1api::serialDeviceInfo> devs;
	for (unsigned int i = 0; i < devices.size(); i++) // for all the connected devices extract information
	{
		fluicell::PPC1api::serialDeviceInfo dev;
		dev.port = devices.at(i).port;
		dev.description = devices.at(i).description;
		dev.hardware_ID = devices.at(i).hardware_id;
		devs.push_back(dev);
		ui_tools->comboBox_serialInfo->addItem(QString::fromStdString(dev.port));
	}

}




void Labonatip_tools::setDefaultPressuresVacuums(int _p_on_default, int _p_off_default, int _v_recirc_default, int _v_switch_default)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::setDefaultPressuresVacuums " 
		<< "  _p_on_default = "  << _p_on_default
		<< "  _p_off_default = " << _p_off_default
		<< "  _v_recirc_default = " << _v_recirc_default
		<< "  _v_switch_default = " << _v_switch_default
		<< endl;

	m_pr_params->p_on_default = _p_on_default;
	m_pr_params->p_off_default = _p_off_default;
	m_pr_params->v_recirc_default = _v_recirc_default;
	m_pr_params->v_switch_default = _v_switch_default;

	ui_tools->spinBox_p_on_default->setValue(_p_on_default);
	ui_tools->spinBox_p_off_default->setValue(_p_off_default);
	ui_tools->spinBox_v_recirc_default->setValue(-_v_recirc_default);
	ui_tools->spinBox_v_switch_default->setValue(-_v_switch_default);
	
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

void Labonatip_tools::languageChanged(int _idx)
{
	language = _idx;
}

void Labonatip_tools::toolButtonStyleChanged(int _idx)
{
	m_GUI_params->showTextToolBar = Qt::ToolButtonStyle(_idx);
}

void Labonatip_tools::enableToolTip(int _inx)
{
	m_GUI_params->enableToolTips = ui_tools->checkBox_enableToolTips->isChecked();
}


void Labonatip_tools::colorSol1Changed(int _value)
{

	float v = _value / 16777216.0;

	//int red = _value & 0x0000FF;
	//int green = (_value >> 8) & 0x0000FF;
	//int blue = (_value >> 16) & 0x0000FF;

	uint32_t color = giveRainbowColor(v);


	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	ui_tools->lineEdit_sol1_name->setPalette(*palette);
	m_solutionParams->sol1_color = QColor::fromRgb(red, green, blue);
	emit colSol1Changed(red, green, blue);

}

void Labonatip_tools::colorSol2Changed(int _value)
{
	float v = _value / 16777216.0;

	//int red = _value & 0x0000FF;
	//int green = (_value >> 8) & 0x0000FF;
	//int blue = (_value >> 16) & 0x0000FF;

	uint32_t color = giveRainbowColor(v);


	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	ui_tools->lineEdit_sol2_name->setPalette(*palette);
	m_solutionParams->sol2_color = QColor::fromRgb(red, green, blue);
	emit colSol2Changed(red, green, blue);
}

void Labonatip_tools::colorSol3Changed(int _value)
{
	float v = _value / 16777216.0;

	//int red = _value & 0x0000FF;
	//int green = (_value >> 8) & 0x0000FF;
	//int blue = (_value >> 16) & 0x0000FF;

	uint32_t color = giveRainbowColor(v);


	int red = color & 0x0000FF;
	int green = (color >> 8) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	//palette.setColor(QPalette::Text, Qt::white);
	ui_tools->lineEdit_sol3_name->setPalette(*palette);
	m_solutionParams->sol3_color = QColor::fromRgb(red, green, blue);
	emit colSol3Changed(red, green, blue);
}

void Labonatip_tools::colorSol4Changed(int _value)
{
	float v = _value / 16777216.0;

	//int red = _value & 0x0000FF;
	//int green = (_value >> 8) & 0x0000FF;
	//int blue = (_value >> 16) & 0x0000FF;

	uint32_t color = giveRainbowColor(v);
	
	
	int red = color & 0x0000FF;
	int green = (color >> 8 ) & 0x0000FF;
	int blue = (color >> 16) & 0x0000FF;

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, QColor::fromRgb(red, green, blue));
	//palette.setColor(QPalette::Text, Qt::white);
	ui_tools->lineEdit_sol4_name->setPalette(*palette);
	m_solutionParams->sol4_color = QColor::fromRgb(red, green, blue);
	emit colSol4Changed(red, green, blue);

}

void Labonatip_tools::getCOMsettingsFromGUI()
{
	m_comSettings->setName (ui_tools->comboBox_serialInfo->currentText().toStdString());
	m_comSettings->setBaudRate ( ui_tools->comboBox_baudRate->currentText().toInt());
	//m_comSettings->dataBits = 
	//m_comSettings->flowControl =
	//m_comSettings->parity =
	//m_comSettings->stopBits =

	//TODO other settings 
}

void Labonatip_tools::getSolutionSettingsFromGUI()
{
	m_solutionParams->vol_well1 = ui_tools->spinBox_vol_sol1->value();
	m_solutionParams->vol_well2 = ui_tools->spinBox_vol_sol2->value();
	m_solutionParams->vol_well3 = ui_tools->spinBox_vol_sol3->value();
	m_solutionParams->vol_well4 = ui_tools->spinBox_vol_sol4->value();
	m_solutionParams->vol_well5 = ui_tools->spinBox_vol_sol5->value();
	m_solutionParams->vol_well6 = ui_tools->spinBox_vol_sol6->value();
	m_solutionParams->vol_well7 = ui_tools->spinBox_vol_sol7->value();
	m_solutionParams->vol_well8 = ui_tools->spinBox_vol_sol8->value();

	m_solutionParams->sol1 = ui_tools->lineEdit_sol1_name->text();
	m_solutionParams->sol2 = ui_tools->lineEdit_sol2_name->text();
	m_solutionParams->sol3 = ui_tools->lineEdit_sol3_name->text();
	m_solutionParams->sol4 = ui_tools->lineEdit_sol4_name->text();

	m_pr_params->base_ds_increment = ui_tools->spinBox_ds_increment->value();
	m_pr_params->base_fs_increment = ui_tools->spinBox_fs_increment->value();
	m_pr_params->base_v_increment = ui_tools->spinBox_v_increment->value();

}

void Labonatip_tools::getGUIsettingsFromGUI()
{
	m_GUI_params->showTextToolBar = Qt::ToolButtonStyle(ui_tools->comboBox_toolButtonStyle->currentIndex());
	m_GUI_params->enableToolTips = ui_tools->checkBox_enableToolTips->isChecked();

}

void Labonatip_tools::getPRsettingsFromGUI()
{

	m_pr_params->p_on_default = ui_tools->spinBox_p_on_default->value();
	m_pr_params->p_off_default = ui_tools->spinBox_p_off_default->value();
	m_pr_params->v_recirc_default = ui_tools->spinBox_v_recirc_default->value();
	m_pr_params->v_switch_default = ui_tools->spinBox_v_switch_default->value();

}

bool Labonatip_tools::loadSettings(QString _path)
{

	if (_path.isEmpty())
	{
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			<< "Warning  ::  Setting file not found \n please set the setting file " << endl;
		//_path = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		//	"Ini file (*.ini);; Data file (*.dat);; Binary File (*.bin);; All Files(*.*)", 0);
	}

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "the path is : " << _path.toStdString() << endl;

	m_settings = new QSettings(_path, QSettings::IniFormat);

	// read default group
	QString user = m_settings->value("default/user", "No owner found").toString();
	ui_tools->lineEdit_userName->setText(user);

	int year = m_settings->value("default/year", "2017").toInt();
	ui_tools->lineEdit_year->setText(QString::number(year));

	QString month = m_settings->value("default/month", "January").toString();
	ui_tools->lineEdit_month->setText(month);

	QString language = m_settings->value("default/language", "english").toString();
	ui_tools->comboBox_language->setCurrentIndex(interpreteLanguage(language));
	languageChanged(ui_tools->comboBox_language->currentIndex());

	// read com group
	//ComName
	QString comPort = m_settings->value("COM/ComName", "COM1").toString();
	m_comSettings->setName ( comPort.toStdString());

	//BaudRate
	int baudRate = m_settings->value("COM/BaudRate", "115200").toInt();
	m_comSettings->setBaudRate(baudRate);
	ui_tools->comboBox_baudRate->setCurrentIndex(7);  // baudrate forced value 115200 

	//DataBits
	int dataBits = m_settings->value("COM/DataBits", "8").toInt();
	switch (dataBits) {
	case 5:
		m_comSettings->setDataBits (serial::fivebits);
		break;
	case 6:
		m_comSettings->setDataBits(serial::sixbits);
		break;
	case 7:
		m_comSettings->setDataBits(serial::sevenbits);
		break;
	case 8:
		m_comSettings->setDataBits(serial::eightbits);
		break;
	default:
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << " Error data bit cannot be read, using default value 8" << endl;
		m_comSettings->setDataBits(serial::eightbits);
		break;
	}

	//Parity = NoParity
	QString parity = m_settings->value("COM/Parity", "NoParity").toString();
	m_comSettings->setParity (serial::parity_none);  //TODO: no intepretation yet

	//StopBits = 1
	int stopBits = m_settings->value("COM/StopBits", "1").toInt();
	m_comSettings->setStopBits(serial::stopbits_one);  //TODO: no intepretation yet

	//FlowControl = noFlow
	QString flowControl = m_settings->value("COM/FlowControl", "noFlow").toString();
	m_comSettings->setFlowControl (serial::flowcontrol_none); //TODO: no intepretation yet

	//read GUI params
	bool ok = false;
	int tbs = m_settings->value("GUI/ToolButtonStyle", "3").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  in GUI params, ToolButtonStyle is corrupted in setting file, using default value " << endl;
	}
	ui_tools->comboBox_toolButtonStyle->setCurrentIndex(tbs);
	m_GUI_params->showTextToolBar = Qt::ToolButtonStyle(tbs);

	//TODO  ENABLE TOOL TIPS NOT READ
	bool enable_tool_tips = m_settings->value("GUI/EnableToolTips", "0").toBool();
	ui_tools->checkBox_enableToolTips->setChecked(enable_tool_tips);
	m_GUI_params->enableToolTips = enable_tool_tips;
	
	// read pr_limits group
	int p_on_max = m_settings->value("pr_limits/p_on_max", "450").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  p_on_max corrupted in setting file, using default value " << endl;
		p_on_max = 450;
	}
	ui_tools->spinBox_p_on_max->setValue(p_on_max);
	m_pr_params->p_on_max = p_on_max;

	int p_on_min = m_settings->value("pr_limits/p_on_min", "0").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  p_on_min corrupted in setting file, using default value " << endl;
		p_on_min = 0;
	}
	ui_tools->spinBox_p_on_min->setValue(p_on_min);
	m_pr_params->p_on_min = p_on_min;

	int p_on_default = m_settings->value("pr_limits/p_on_default", "190").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  p_on_default corrupted in setting file, using default value " << endl;
		p_on_default = 190;
	}
	ui_tools->spinBox_p_on_default->setValue(p_on_default);
	m_pr_params->p_on_default = p_on_default;

	int p_off_max = m_settings->value("pr_limits/p_off_max", "450").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  p_off_max corrupted in setting file, using default value " << endl;
		p_off_max = 450;
	}
	ui_tools->spinBox_p_off_max->setValue(p_off_max); //TODO
	m_pr_params->p_off_max = p_off_max;

	int p_off_min = m_settings->value("pr_limits/p_off_min", "0").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  p_off_min corrupted in setting file, using default value " << endl;
		p_off_min = 0;
	}
	ui_tools->spinBox_p_off_min->setValue(p_off_min);
	m_pr_params->p_off_min = p_off_min;

	int p_off_default = m_settings->value("pr_limits/p_off_default", "21").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  p_off_default corrupted in setting file, using default value " << endl;
		p_off_default = 21;
	}
	ui_tools->spinBox_p_off_default->setValue(p_off_default);
	m_pr_params->p_off_default = p_off_default;

	int v_switch_max = m_settings->value("pr_limits/v_switch_max", "0").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  v_switch_max corrupted in setting file, using default value " << endl;
		v_switch_max = 0;
	}
	ui_tools->spinBox_v_switch_max->setValue(v_switch_max);
	m_pr_params->v_switch_max = v_switch_max;

	int v_switch_min = m_settings->value("pr_limits/v_switch_min", "-300").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  v_switch_min corrupted in setting file, using default value " << endl;
		v_switch_min = -300;
	}
	ui_tools->spinBox_v_switch_min->setValue(v_switch_min);
	m_pr_params->v_switch_min = v_switch_min;

	int v_switch_default = m_settings->value("pr_limits/v_switch_default", "-115").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  v_switch_default corrupted in setting file, using default value " << endl;
		v_switch_default = -115;
	}
	ui_tools->spinBox_v_switch_default->setValue(v_switch_default);
	m_pr_params->v_switch_default = v_switch_default;

	int v_recirc_max = m_settings->value("pr_limits/v_recirc_max", "0").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  v_recirc_max corrupted in setting file, using default value " << endl;
		v_recirc_max = 0;
	}
	ui_tools->spinBox_v_recirc_max->setValue(v_recirc_max);
	m_pr_params->v_recirc_max = v_recirc_max;

	int v_recirc_min = m_settings->value("pr_limits/v_recirc_min", "-300").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  v_recirc_min corrupted in setting file, using default value " << endl;
		v_recirc_min = -300;
	}
	ui_tools->spinBox_v_recirc_min->setValue(v_recirc_min);
	m_pr_params->v_recirc_min = v_recirc_min;

	int v_recirc_default = m_settings->value("pr_limits/v_recirc_default", "-115").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  v_recirc_default corrupted in setting file, using default value " << endl;
		v_recirc_default = -115;
	}
	ui_tools->spinBox_v_recirc_default->setValue(v_recirc_default);
	m_pr_params->v_recirc_default = v_recirc_default;

	int base_ds_increment = m_settings->value("pr_limits/base_ds_increment", "10").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  base_ds_increment corrupted in setting file, using default value " << endl;
		base_ds_increment = 10;
	}
	ui_tools->spinBox_ds_increment->setValue(base_ds_increment);
	m_pr_params->base_ds_increment = base_ds_increment;

	int base_fs_increment = m_settings->value("pr_limits/base_fs_increment", "5").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  base_fs_increment corrupted in setting file, using default value " << endl;
		base_fs_increment = 5;
	}
	ui_tools->spinBox_fs_increment->setValue(base_fs_increment);
	m_pr_params->base_fs_increment = base_fs_increment;

	int base_v_increment = m_settings->value("pr_limits/base_v_increment", "5").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  base_v_increment corrupted in setting file, using default value " << endl;
		base_v_increment = 5;
	}
	ui_tools->spinBox_v_increment->setValue(base_v_increment);
	m_pr_params->base_v_increment = base_v_increment;

	// read server group


	//Read solution volumes block
	int vol_sol1 = m_settings->value("solutions/volWell1", "30").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 1 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol1->setValue(vol_sol1);
	m_solutionParams->vol_well1 = vol_sol1;

	int vol_sol2 = m_settings->value("solutions/volWell2", "30").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 2 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol2->setValue(vol_sol2);
	m_solutionParams->vol_well2 = vol_sol2;

	int vol_sol3 = m_settings->value("solutions/volWell3", "30").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 3 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol3->setValue(vol_sol3);
	m_solutionParams->vol_well3 = vol_sol3;


	int vol_sol4 = m_settings->value("solutions/volWell4", "30").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 4 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol4->setValue(vol_sol4);
	m_solutionParams->vol_well4 = vol_sol4;


	int vol_sol5 = m_settings->value("solutions/volWell5", "35").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 4 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol5->setValue(vol_sol5);
	m_solutionParams->vol_well5 = vol_sol5;

	int vol_sol6 = m_settings->value("solutions/volWell6", "35").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 4 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol6->setValue(vol_sol6);
	m_solutionParams->vol_well6 = vol_sol6;

	int vol_sol7 = m_settings->value("solutions/volWell7", "35").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 4 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol7->setValue(vol_sol7);
	m_solutionParams->vol_well7 = vol_sol7;

	int vol_sol8 = m_settings->value("solutions/volWell8", "35").toInt(&ok);
	if (!ok) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::loadSettings ::: Warning  ::  volume of solution 4 corrupted in setting file, using default value " << endl;
	}
	ui_tools->spinBox_vol_sol8->setValue(vol_sol8);
	m_solutionParams->vol_well8 = vol_sol8;


	//Read solution names block
	QString solname1 = m_settings->value("solutions/sol1", "no name").toString();
	ui_tools->lineEdit_sol1_name->setText(solname1);
	m_solutionParams->sol1 = solname1; 

	QString solname2 = m_settings->value("solutions/sol2", "no name").toString();
	ui_tools->lineEdit_sol2_name->setText(solname2);
	m_solutionParams->sol2 = solname2; 

	QString solname3 = m_settings->value("solutions/sol3", "no name").toString();
	ui_tools->lineEdit_sol3_name->setText(solname3);
	m_solutionParams->sol3 = solname3; 

	QString solname4 = m_settings->value("solutions/sol1", "no name").toString();
	ui_tools->lineEdit_sol4_name->setText(solname4);
	m_solutionParams->sol4 = solname4; 

	int sol1colSlider = m_settings->value("solutions/sol1colSlider", "3522620").toInt();
	ui_tools->horizontalSlider_colorSol1->setValue(sol1colSlider);
	colorSol1Changed(sol1colSlider);

		
	int sol2colSlider = m_settings->value("solutions/sol2colSlider", "5164400").toInt();
	ui_tools->horizontalSlider_colorSol2->setValue(sol2colSlider);
	colorSol2Changed(sol2colSlider);

	int sol3colSlider = m_settings->value("solutions/sol3colSlider", "12926220").toInt();
	ui_tools->horizontalSlider_colorSol3->setValue(sol3colSlider);
	colorSol3Changed(sol3colSlider);

	int sol4colSlider = m_settings->value("solutions/sol4colSlider", "1432930").toInt();
	ui_tools->horizontalSlider_colorSol4->setValue(sol4colSlider);
	colorSol4Changed(sol4colSlider);

	return true;
}

bool Labonatip_tools::saveSettings(QString _file_name)
{

	//m_settings->setPath(QSettings::IniFormat, QSettings::UserScope, _file_name);
	QSettings *settings = new QSettings(_file_name, QSettings::IniFormat);
	// [default]
	// user = 
	settings->setValue("default/user", ui_tools->lineEdit_userName->text());
	// year = 
	settings->setValue("default/year", ui_tools->lineEdit_year->text());
	// month = 
	settings->setValue("default/month", ui_tools->lineEdit_month->text());
	// language = 
	settings->setValue("default/language", ui_tools->comboBox_language->currentText());

	// [COM]
	// ComName = COM_
	settings->setValue("COM/ComName", QString::fromStdString(m_comSettings->getName())); //TODO weird that here I get the actual setting
	// BaudRate = 115200
	settings->setValue("COM/BaudRate", ui_tools->comboBox_baudRate->currentText());//TODO whereas here I get the GUI value
	// DataBits = 8
	settings->setValue("COM/DataBits", ui_tools->comboBox_dataBit->currentText());
	// Parity = NoParity
	settings->setValue("COM/Parity", ui_tools->comboBox_parity->currentText());
	// StopBits = 1
	settings->setValue("COM/StopBits", ui_tools->comboBox_stopBit->currentText());
	// FlowControl = noFlow
	settings->setValue("COM/FlowControl", ui_tools->comboBox_flowControl->currentText());

	// [GUI]
	settings->setValue("GUI/ToolButtonStyle", ui_tools->comboBox_toolButtonStyle->currentIndex());
	settings->setValue("GUI/enableToolTips", ui_tools->checkBox_enableToolTips->isChecked());

	// [pr_limits]
	// p_on_max = 
	settings->setValue("pr_limits/p_on_max", ui_tools->spinBox_p_on_max->value());
	// p_on_min =
	settings->setValue("pr_limits/p_on_min", ui_tools->spinBox_p_on_min->value());
	// p_on_default = 
	settings->setValue("pr_limits/p_on_default", ui_tools->spinBox_p_on_default->value());
	// p_off_max = 
	settings->setValue("pr_limits/p_off_max", ui_tools->spinBox_p_off_max->value());
	// p_off_min = 
	settings->setValue("pr_limits/p_off_min", ui_tools->spinBox_p_off_min->value());
	// p_off_default = 
	settings->setValue("pr_limits/p_off_default", ui_tools->spinBox_p_off_default->value());
	// v_switch_max = 
	settings->setValue("pr_limits/v_switch_max", ui_tools->spinBox_v_switch_max->value());
	// v_switch_min = 
	settings->setValue("pr_limits/v_switch_min", ui_tools->spinBox_v_switch_min->value());
	// v_switch_default = 
	settings->setValue("pr_limits/v_switch_default", ui_tools->spinBox_v_switch_default->value());
	// v_recirc_max = 
	settings->setValue("pr_limits/v_recirc_max", ui_tools->spinBox_v_recirc_max->value());
	// v_recirc_min = 
	settings->setValue("pr_limits/v_recirc_min", ui_tools->spinBox_v_recirc_min->value());
	// v_recirc_default = 
	settings->setValue("pr_limits/v_recirc_default", ui_tools->spinBox_v_recirc_default->value());
	// base_ds_increment = 
	settings->setValue("pr_limits/base_ds_increment", ui_tools->spinBox_ds_increment->value());
	// base_ds_increment = 
	settings->setValue("pr_limits/base_fs_increment", ui_tools->spinBox_fs_increment->value());
	// base_ds_increment = 
	settings->setValue("pr_limits/base_v_increment", ui_tools->spinBox_v_increment->value());


	// [Well volumes]
	// well 1
	settings->setValue("solutions/volWell1", ui_tools->spinBox_vol_sol1->value());
	// well 2
	settings->setValue("solutions/volWell2", ui_tools->spinBox_vol_sol2->value());
	// well 3
	settings->setValue("solutions/volWell3", ui_tools->spinBox_vol_sol3->value());
	// well 4
	settings->setValue("solutions/volWell4", ui_tools->spinBox_vol_sol4->value());
	// well 5
	settings->setValue("solutions/volWell5", ui_tools->spinBox_vol_sol5->value());
	// well 6
	settings->setValue("solutions/volWell6", ui_tools->spinBox_vol_sol6->value());
	// well 7
	settings->setValue("solutions/volWell7", ui_tools->spinBox_vol_sol7->value());
	// well 8
	settings->setValue("solutions/volWell8", ui_tools->spinBox_vol_sol8->value());

	// [solutionNames]
	// solution1 = CuSO4
	settings->setValue("solutions/sol1", ui_tools->lineEdit_sol1_name->text());
	// solution2 = NaCl
	settings->setValue("solutions/sol2", ui_tools->lineEdit_sol2_name->text());
	// solution3 = NaHCO3
	settings->setValue("solutions/sol3", ui_tools->lineEdit_sol3_name->text());
	// solution4 = FeS
	settings->setValue("solutions/sol4", ui_tools->lineEdit_sol4_name->text());
	// sol1colSlider
	settings->setValue("solutions/sol1colSlider", ui_tools->horizontalSlider_colorSol1->value());
	// sol2colSlider
	settings->setValue("solutions/sol2colSlider", ui_tools->horizontalSlider_colorSol2->value());
	// sol3colSlider
	settings->setValue("solutions/sol3colSlider", ui_tools->horizontalSlider_colorSol3->value());
	// sol4colSlider
	settings->setValue("solutions/sol4colSlider", ui_tools->horizontalSlider_colorSol4->value());

	settings->sync();
	//m_settings->sync();
	
	return true;
}


void Labonatip_tools::resetToDefaultValues()
{

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_warning, m_str_factory_reset,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		return;
	}
	else {
		loadSettings("");
	}

	
}


int Labonatip_tools::interpreteLanguage(QString _language)
{
	if (_language == "Chinese")
	{
		return 0;
	}
	if (_language == "English")
	{
		return 1;
	}
	if (_language == "Italiano")
	{
		return 2;
	}
	if (_language == "Svenska")
	{
		return 3;
	}
	return 0;
}



uint32_t Labonatip_tools::giveRainbowColor(float _position)
{

	// this function gives 1D linear RGB color gradient
	// color is proportional to position
	// position  <0;1>
	// position means position of color in color gradient

	if (_position>1) _position = 1;//position-int(position);
								// if position > 1 then we have repetition of colors
								// it maybe useful
	uint8_t R = 0;// byte
	uint8_t G = 0;// byte
	uint8_t B = 0;// byte
	int nmax = 6;// number of color bars
	float m = nmax* _position;
	int n = int(m); // integer of m
	float f = m - n;  // fraction of m
	uint8_t t = int(f * 255);


	switch (n) {
	case 0:
	{
		R = 0;
		G = 255;
		B = t;
		break;
	}

	case 1:
	{
		R = 0;
		G = 255 - t;
		B = 255;
		break;
	}
	case 2:
	{
		R = t;
		G = 0;
		B = 255;
		break;
	}
	case 3:
	{
		R = 255;
		G = 0;
		B = 255 - t;
		break;
	}
	case 4:
	{
		R = 255;
		G = t;
		B = 0;
		break;
	}
	case 5: {
		R = 255 - t;
		G = 255;
		B = 0;
		break;
	}
	case 6:
	{
		R = 0;
		G = 255;
		B = 0;
		break;
	}

	}; // case


	return (R << 16) | (G << 8) | B;
}



void Labonatip_tools::switchLanguage(QString _translation_file)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_tools::switchLanguage " << endl;
	
	qApp->removeTranslator(&m_translator_tool);

	if (m_translator_tool.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_tool);

		ui_tools->retranslateUi(this); 

		m_str_warning = QApplication::translate("Labonatip_GUI", qPrintable(m_str_warning));
		m_str_factory_reset = QApplication::translate("Labonatip_GUI", qPrintable(m_str_factory_reset));

		cout << QDate::currentDate().toString().toStdString() << "  "
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::switchLanguage   installTranslator" << endl;
	}

}


Labonatip_tools::~Labonatip_tools() {
	delete ui_tools;
}