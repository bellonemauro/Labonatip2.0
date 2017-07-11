/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
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

	ui_tools->comboBox_serialInfo->clear();
	connect(ui_tools->comboBox_serialInfo,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
		&Labonatip_tools::showPortInfo);

	// enumerate connected com ports
	enumerate();

	// connect GUI elements: communication tab
	connect(ui_tools->pushButton_enumerate,
		SIGNAL(clicked()), this, SLOT(enumerate()));

	// connect GUI elements: macro tab

	connect(ui_tools->treeWidget_macroTable, 
		SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, 
		SLOT(checkValidity(QTreeWidgetItem *, int)));

	connect(ui_tools->listWidget_options,
		SIGNAL(itemClicked(QListWidgetItem*)), this, 
		SLOT(onListMailItemClicked(QListWidgetItem*)));

	// connect GUI elements: NEW macro tab
	connect(ui_tools->pushButton_addMacroCommand,
		SIGNAL(clicked()), this, SLOT(addMacroCommand()));

	connect(ui_tools->pushButton_removeMacroCommand,
		SIGNAL(clicked()), this, SLOT(removeMacroCommand()));

	connect(ui_tools->pushButton_becomeChild,
		SIGNAL(clicked()), this, SLOT(becomeChild()));

	connect(ui_tools->pushButton_becomeParent,
		SIGNAL(clicked()), this, SLOT(becomeParent()));

	connect(ui_tools->pushButton_moveDown,
		SIGNAL(clicked()), this, SLOT(moveDown()));

	connect(ui_tools->pushButton_moveUp,
		SIGNAL(clicked()), this, SLOT(moveUp()));

	connect(ui_tools->pushButton_plusIndent,
		SIGNAL(clicked()), this, SLOT(plusIndent()));

	connect(ui_tools->pushButton_duplicateLine,
		SIGNAL(clicked()), this, SLOT(duplicateItem()));

	connect(ui_tools->pushButton_clearCommands,
		SIGNAL(clicked()), this, SLOT(clearAllCommands()));

	connect(ui_tools->pushButton_saveMacro,
		SIGNAL(clicked()), this, SLOT(saveMacro()));

	connect(ui_tools->pushButton_loadMacro, 
		SIGNAL(clicked()), this, SLOT(loadMacro()));


	// connect tool window events Ok, Discard, Apply
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
}

void Labonatip_tools::okPressed() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::okPressed " << endl;

	getCOMsettings();
	getSolutionSettings();
	addAllCommandsToMacro();

	saveSettings();
	//TODO other settings ! 

	emit ok();
	this->close();
}

void Labonatip_tools::discardPressed() {
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::discardPressed " << endl;
	emit discard();
	this->close();
}

void Labonatip_tools::applyPressed() {
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::applyPressed " << endl;
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



void Labonatip_tools::addMacroCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::addMacroCommand " << endl;
	
	// create a new item
	QTreeWidgetItem *newItem = new QTreeWidgetItem;
	QComboBox *comboBox = new QComboBox(this);
	createNewCommand(*newItem, *comboBox);

	//if we are at the top level with no element or no selection 
	// the element is added at the last position
	if (ui_tools->treeWidget_macroTable->topLevelItemCount() < 1 ||
		!ui_tools->treeWidget_macroTable->currentIndex().isValid()) {
		ui_tools->treeWidget_macroTable->insertTopLevelItem(0, newItem);
		// the combo widget must be created for every item in the tree 
		// and it goes always to the first column
		ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox); // set the combowidget
		
		return;
	}
	else { 	//else we add the item at a specific row
		int row = ui_tools->treeWidget_macroTable->currentIndex().row();
		// get the parent
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();

		if (ui_tools->treeWidget_macroTable->currentItem()->parent()) { // if the parent is valid
			// add the new line as a child
			parent->insertChild(row + 1, newItem);
			ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);// set the combowidget
		}
		else { // add the new line at the row 
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row + 1, newItem);
			ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);// set the combowidget
		}

		return;
	}

}

void Labonatip_tools::removeMacroCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::removeMacroCommand " << endl;

	if (ui_tools->treeWidget_macroTable->currentItem() &&
		ui_tools->treeWidget_macroTable->topLevelItemCount() > 0) {// avoid crash is no elements in the table or no selection
		// destroy the selected item
		ui_tools->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
}

void Labonatip_tools::becomeChild()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::becomeChild " << endl;

	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection
	
    // create a new item
	QTreeWidgetItem *item = new QTreeWidgetItem();
	QComboBox *comboBox = new QComboBox(this);
	createNewCommand(*item, *comboBox); 
	
	// clone the current selected item
	item = ui_tools->treeWidget_macroTable->currentItem()->clone();
	//get the row index
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();

	// the item above the current item becomes the parent
	QTreeWidgetItem *parent = 
		ui_tools->treeWidget_macroTable->itemAbove(
			ui_tools->treeWidget_macroTable->currentItem());
	if(parent){
		parent->addChild(item); // add the clone of the selected item as a child
		ui_tools->treeWidget_macroTable->setItemWidget(item, 0, comboBox); // set the combowidget
		parent->setExpanded(true);
		// destroy the selected item
		ui_tools->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
}

void Labonatip_tools::becomeParent()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::becomeParent " << endl;

	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection

	QTreeWidgetItem *item = new QTreeWidgetItem;
	QComboBox *comboBox = new QComboBox(this);
	createNewCommand(*item, *comboBox);

	item = ui_tools->treeWidget_macroTable->currentItem()->clone();
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
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::moveUp " << endl;

	// create a combo
	QComboBox *comboBox = new QComboBox(this);
	createNewCommand(*comboBox);

	// get the current selected item
	QTreeWidgetItem *moveItem = ui_tools->treeWidget_macroTable->currentItem();
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();

	if (moveItem && row > 0) // if the selection is valid and we are not at the first row
	{
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();
		// if we are not at the fist level, so the item has a parent
		if (parent) {
			parent->takeChild(row); // take the child at the row
			parent->insertChild(row - 1, moveItem); // add the selected item one row before
			ui_tools->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox); // set the combowidget
		}
		else {
			// if we are on the top level, just take the item 
			ui_tools->treeWidget_macroTable->takeTopLevelItem(row);
			// and add the selected item one row before
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row - 1, moveItem);
			ui_tools->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox); // set the combowidget
		}
		ui_tools->treeWidget_macroTable->setCurrentItem(moveItem);
	}
}

void Labonatip_tools::moveDown()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::moveDown " << endl;

	// create a combo
	QComboBox *comboBox = new QComboBox(this);
	createNewCommand(*comboBox);

	// get the current selected item
	QTreeWidgetItem *moveItem = ui_tools->treeWidget_macroTable->currentItem();

	int row = ui_tools->treeWidget_macroTable->currentIndex().row();
	int number_of_items = ui_tools->treeWidget_macroTable->topLevelItemCount();

	if (moveItem && row >= 0 && row < number_of_items - 1)
	{
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();

		if (parent) {
			int childCount = parent->childCount();
			if (row >= childCount - 1) return; // stops at the last

			parent->takeChild(row);
			parent->insertChild(row + 1, moveItem);
			ui_tools->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox);
		}
		else {
			ui_tools->treeWidget_macroTable->takeTopLevelItem(row);
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row + 1, moveItem);
			ui_tools->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox);
		}
		ui_tools->treeWidget_macroTable->setCurrentItem(moveItem);
	}
}


void Labonatip_tools::plusIndent()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::plusIndent " << endl;

	// create a new item
	QTreeWidgetItem *newItem = new QTreeWidgetItem;
	QComboBox *comboBox = new QComboBox(this);
	createNewCommand(*newItem, *comboBox);

    // if no item selected, add to the top level
	if (!ui_tools->treeWidget_macroTable->currentIndex().isValid()) {
		ui_tools->treeWidget_macroTable->insertTopLevelItem(0, newItem);
		ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox); // set the combowidget
		return;
	}
	else { // otherwise it add the item as a child
		QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem();
		parent->insertChild(0, newItem);
		ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox); // set the combowidget
		return;
	}

}



bool Labonatip_tools::checkValidity(QTreeWidgetItem *_item, int _column)
{
	// check validity for the element
	int row = ui_tools->treeWidget_macroTable->currentIndex().row();
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_tools::checkValidity ::: check validity on row " << row 
		 << " and column " << _column << endl;

	//if (_column != 1) return false; // perform the check on column 1 only
	// in this way the check will be called any modification 
	// but the check will be performed always on the column number 1
	_column = 1;  
	int idx = 
		qobject_cast<QComboBox*>(
			ui_tools->treeWidget_macroTable->itemWidget(_item, 0))->currentIndex();
	
	bool isNumeric;

	switch (idx) {
	case 0: { // check pon
		
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : "  << idx << endl;

		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) { // if is not a number 
			QMessageBox::warning(this, "Warning", 
				"Pressure ON is not a valid number, \n its value must be a positive number in [0, 450]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < m_pr_params->p_on_min || 
			number > m_pr_params->p_on_max) { // if is not the range
			QMessageBox::warning(this, "Warning", 
				" Pressure ON is out of range, \n its value must be a positive number in [0, 450]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 1: { // check poff
		
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;

		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) { // if is not a number
			QMessageBox::warning(this, "Warning", 
				"Pressure OFF is not a valid number, \n its value must be a positive number in [0, 450]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < m_pr_params->p_off_min || 
			number > m_pr_params->p_off_max) { // if is not the range
			QMessageBox::warning(this, "Warning", 
				" Pressure ON is out of range, \n its value must be a positive number in [0, 450]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 2:{// check v_s

		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : " << idx << endl;

		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) { // if is not a number
			QMessageBox::warning(this, "Warning", 
				"Vacuum switch is not a valid number, \n its value must be a positive number in [-300, 0]"); 
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < m_pr_params->v_switch_min || 
			number > m_pr_params->v_switch_max) { // if is not the range
			QMessageBox::warning(this, "Warning", 
				"Vacuum switch is out of range, \n its value must be a positive number in [-300, 0]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 3: {
		// check v_r

		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : " << idx << endl;
		
		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning", 
				"Vacuum recirculation is not a valid number, \n its value must be a positive number in [-300, 0]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < m_pr_params->v_recirc_min || 
			number > m_pr_params->v_recirc_max) { // if is not the range
			QMessageBox::warning(this, "Warning", 
				"Vacuum recirculation is out of range, \n its value must be a positive number in [-300, 0]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 4: case 5: case 6: case 7: { //from 4 to 7
		// check open valve : 0 = no valve, 1,2,3,4 valves 1,2,3,4

		cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::checkValidity ::: case : " << idx << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Solution is not a valid number, \n its value must be 0 or 1 ! \n where 0 = open. \nOnly one valve can be open.");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number != 0 && 
			number != 1) {
			QMessageBox::warning(this, "Warning !", 
				" Solution is out of range, \n its value must be 0 or 1 ! \n where 0 = open. \nOnly one valve can be open.");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 8:{
		// Droplet size (%)
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : " << idx << endl;

		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning",
				"Droplet size percentage is not a valid number, \n its value must be a positive number in [0, 200]"); //TODO limit?
			_item->setText(_column, QString("100")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < 0 ||
			number > 200) { // if is not the range
			QMessageBox::warning(this, "Warning",
				"Droplet size percentage is out of range, \n its value must be a positive number in [0, 200]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}   
	case 9:{
		// Flow speed (%)
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : " << idx << endl;

		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning",
				"Flow speed percentage is not a valid number, \n its value must be a positive number in [0, 200]"); //TODO limit?
			_item->setText(_column, QString("100")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < 0 ||
			number > 200) { // if is not the range
			QMessageBox::warning(this, "Warning",
				"Flow speed percentage is out of range, \n its value must be a positive number in [0, 200]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 10: {
		// Vacuum (%)
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : " << idx << endl;
		
		// get the number to be checked
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning",
				"Vacuum percentage is not a valid number, \n its value must be a positive number in [0, 200]"); //TODO limit?
			_item->setText(_column, QString("100")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < 0 ||
			number > 200) { // if is not the range
			QMessageBox::warning(this, "Warning",
				"Vacuum percentage is out of range, \n its value must be a positive number in [0, 200]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 11: {
		// check loops

		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::checkValidity ::: case : " << idx << endl;

		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Loop is not a valid number, \n value must be a positive integer number!");
			_item->setText(_column, QString("1"));
			return false;
		}
		if (number < 1 ) { // if is not the range
			QMessageBox::warning(this, "Warning",
				"Loop is out of range, \n its value must be a positive number");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 12: {
		// check Wait (s)

		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;
		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning !", " Loop is not a valid number, \n value must be a positive integer number!");
			_item->setText(_column, QString("1"));
			return false;
		}
		if (number < 1) { // if is not the range
			QMessageBox::warning(this, "Warning",
				"Waiting time is out of range, \n its value must be a positive number ");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 13: {
		// ask
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;

		break;
	}
	case 14: {
		// all off
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;

		break;
	}
	case 15: {
		// pumps off
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl; 

		break;
	}	
	case 16: {
		// Valve state"
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;

		break;
	}	
	case 17: {
		// Wait sync"
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;

		break;
	}	
	case 18: {
		// Sync out"
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "case : " << idx << endl;

		break;
	}
	default:{
		// default function active if none of the previous
		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "default : " << idx << endl;
		break;
	}
	}

	return true;
}

void Labonatip_tools::commandChanged(int _idx)
{
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_tools::commandChanged :::: " << _idx << endl;

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_tools::commandChanged :::: row " 
		 << ui_tools->treeWidget_macroTable->currentIndex().row() << endl;

	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection
																 
	// TODO: retrive the right row where the event is generated
	// get the index in the combobox of the current item and set it to the new widget
//	QTreeWidget *wid = qobject_cast<QTreeWidget*>(
//		_combo_box.parentWidget());

	int idx = qobject_cast<QComboBox*>(
		ui_tools->treeWidget_macroTable->focusWidget())->currentIndex();

	int row  = ui_tools->treeWidget_macroTable->indexAt(
		ui_tools->treeWidget_macroTable->focusWidget()->pos()).row();

	int column = ui_tools->treeWidget_macroTable->indexAt(
		ui_tools->treeWidget_macroTable->focusWidget()->pos()).column();

	QTreeWidgetItem *currentItem = ui_tools->treeWidget_macroTable->itemAt(
		ui_tools->treeWidget_macroTable->focusWidget()->pos());

	ui_tools->treeWidget_macroTable->itemAt(
		ui_tools->treeWidget_macroTable->focusWidget()->pos())->setText(0, QString::number(idx));

	QTreeWidgetItem *currentParent = currentItem->parent();
	int parent = 0;
	if (currentParent) parent = 1;
	//int idx = 1;// ui_tools->treeWidget_macroTable->indexOfTopLevelItem(wid);
		//_combo_box.currentIndex();
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::commandChanged :::: idx "
		<< idx 
		<< "  row " << row 
		<< "  column " << column
		<< "  currentParent " << parent
		<< endl;

}


void Labonatip_tools::duplicateItem()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::duplicateItem :::: " << endl;

	if (!ui_tools->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection

	// get the current selected item
	QTreeWidgetItem *newItem = 
		ui_tools->treeWidget_macroTable->currentItem()->clone();

	int row = ui_tools->treeWidget_macroTable->currentIndex().row();
	QTreeWidgetItem *parent = ui_tools->treeWidget_macroTable->currentItem()->parent();

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::duplicateItem :::: has valid element selected at row " << row << endl;

	if (parent) {
			cout << QDate::currentDate().toString().toStdString() << "  "
				<< QTime::currentTime().toString().toStdString() << "  "
				<< "Labonatip_tools::duplicateItem :::: if has parent  " << endl;

			parent->insertChild(row + 1, newItem);
			// create a combo
			QComboBox *comboBox = new QComboBox(this);
			createNewCommand(*comboBox);
			// get the index in the combobox of the current item and set it to the new widget
			int idx = qobject_cast<QComboBox*>(
				ui_tools->treeWidget_macroTable->itemWidget(
					ui_tools->treeWidget_macroTable->currentItem(), 0))->currentIndex();
			comboBox->blockSignals(true);
			comboBox->setCurrentIndex(idx);
			comboBox->blockSignals(false);
			ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
		}
		else {
			cout << QDate::currentDate().toString().toStdString() << "  "
				<< QTime::currentTime().toString().toStdString() << "  "
				<< "Labonatip_tools::duplicateItem :::: else no parent " << endl;
			
			ui_tools->treeWidget_macroTable->insertTopLevelItem(row + 1, newItem);
			// create a combo
			QComboBox *comboBox = new QComboBox(this);
			createNewCommand(*comboBox);
			// get the index in the combobox of the current item and set it to the new widget
			int idx = qobject_cast<QComboBox*>(
				ui_tools->treeWidget_macroTable->itemWidget(
					ui_tools->treeWidget_macroTable->currentItem(), 0))->currentIndex();
			comboBox->blockSignals(true);
			comboBox->setCurrentIndex(idx);
			comboBox->blockSignals(false);

			ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
			if (newItem->childCount() > 0)
			{
				for (int i = 0; i < newItem->childCount(); i++)	{

					// create a new combobox for each child
					QComboBox *comboBox_child = new QComboBox(this);
					createNewCommand(*comboBox_child);
					// get the index in the combobox of the current item and set it to the new widget
					int idx = qobject_cast<QComboBox*>(
						ui_tools->treeWidget_macroTable->itemWidget(
							ui_tools->treeWidget_macroTable->currentItem()->child(i), 0))->currentIndex();
					
					comboBox_child->blockSignals(true);
					comboBox_child->setCurrentIndex(idx);
					comboBox_child->blockSignals(false);
					ui_tools->treeWidget_macroTable->setItemWidget(
						newItem->child(i), 0, comboBox_child);
				}
			}
//		}
		return;
		//TODO all the children do not have the combobox
	}
	return;
}

void Labonatip_tools::createNewCommand(QTreeWidgetItem & _command, QComboBox & _combo_box)
{

	_command.setText(0, "Command"); // 
	_command.setText(1, "1"); // 
	_command.setCheckState(2, Qt::CheckState::Unchecked); // status message
	_command.setText(2, "go science !!!"); // status message
	_command.setFlags(_command.flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));

	_combo_box.addItems(QStringList() << "Pressure ON (mbar)" << "Pressure OFF (mbar)" 
		<< "Vacuum Switch (mbar)" << "Vacuum Recirculation (mbar)"
		<< "Solution 1 (open/close)" << "Solution 2 (open/close)" 
		<< "Solution 3 (open/close)" << "Solution 4 (open/close)"
		<< "Droplet size (%)" << "Flow speed (%)" << "Vacuum (%)" 
		<< "loop" << "Sleep (s)" << "Aks"
		<< "All Off" << "Pumps Off" << "Valve state" << "Wait sync" << "Sync out");

	connect(&_combo_box, SIGNAL(currentIndexChanged(int )), 
		this, SLOT(commandChanged(int )));
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
	m_comSettings->name = ui_tools->comboBox_serialInfo->currentText().toStdString();
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
		cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< "Warning !  ::  Setting file not found !\n please set the setting file " << endl;
		//_path = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		//	"Ini file (*.ini);; Data file (*.dat);; Binary File (*.bin);; All Files(*.*)", 0);
	}

	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		<< "the path is : " << _path.toStdString() << endl;

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
	m_comSettings->name = comPort.toStdString();

	//BaudRate
	int baudRate = m_settings->value("COM/BaudRate", "115200").toInt();
	m_comSettings->baudRate = baudRate;
	ui_tools->comboBox_baudRate->setCurrentIndex(7);  // baudrate forced value 115200 

	//DataBits
	int dataBits = m_settings->value("COM/DataBits", "8").toInt();
	switch (dataBits) {
	case 5:
		m_comSettings->dataBits = serial::fivebits;
		break;
	case 6:
		m_comSettings->dataBits = serial::sixbits;
		break;
	case 7:
		m_comSettings->dataBits = serial::sevenbits;
		break;
	case 8:
		m_comSettings->dataBits = serial::eightbits;
		break;
	default:
		cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< " Error data bit cannot be read, using default value 8" << endl;
		m_comSettings->dataBits = serial::eightbits;
		break;
	}

	//Parity = NoParity
	QString parity = m_settings->value("COM/Parity", "NoParity").toString();
	m_comSettings->parity = serial::parity_none;  //TODO: no intepretation yet

	//StopBits = 1
	int stopBits = m_settings->value("COM/StopBits", "1").toInt();
	m_comSettings->stopBits = serial::stopbits_one;  //TODO: no intepretation yet

	//FlowControl = noFlow
	QString flowControl = m_settings->value("COM/FlowControl", "noFlow").toString();
	m_comSettings->flowControl = serial::flowcontrol_none; //TODO: no intepretation yet

	
	// read pr_limits group
	int p_on_max = m_settings->value("pr_limits/p_on_max", "450").toInt();
	ui_tools->lineEdit_p_on_max->setText(QString::number(p_on_max));
	m_pr_params->p_on_max = p_on_max;

	int p_on_min = m_settings->value("pr_limits/p_on_min", "0").toInt();
	ui_tools->lineEdit_p_on_min->setText(QString::number(p_on_min));
	m_pr_params->p_on_min = p_on_min;

	int p_on_default = m_settings->value("pr_limits/p_on_default", "0").toInt();
	ui_tools->lineEdit_p_on_default->setText(QString::number(p_on_default));
	m_pr_params->p_on_default = p_on_default;

	int p_off_max = m_settings->value("pr_limits/p_off_max", "450").toInt();
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

	int v_switch_min = m_settings->value("pr_limits/v_switch_min", "-300").toInt();
	ui_tools->lineEdit_v_switch_min->setText(QString::number(v_switch_min));
	m_pr_params->v_switch_min = v_switch_min;

	int v_switch_default = m_settings->value("pr_limits/v_switch_default", "-115").toInt();
	ui_tools->lineEdit_v_switch_default->setText(QString::number(v_switch_default));
	m_pr_params->v_switch_default = v_switch_default;

	int v_recirc_max = m_settings->value("pr_limits/v_recirc_max", "0").toInt();
	ui_tools->lineEdit_v_recirc_max->setText(QString::number(v_recirc_max));
	m_pr_params->v_recirc_max = v_recirc_max;

	int v_recirc_min = m_settings->value("pr_limits/v_recirc_min", "-300").toInt();
	ui_tools->lineEdit_v_recirc_min->setText(QString::number(v_recirc_min));
	m_pr_params->v_recirc_min = v_recirc_min;

	int v_recirc_default = m_settings->value("pr_limits/v_recirc_default", "-115").toInt();
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


	//cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
	//	<< " Load settings: \n"
	//	<< "    default/owner " << user.toStdString() << " \n"
	//	<< "    default/date " << year << " \n"
	//	<< "    default/month " << month.toStdString() << " \n\n"
	//	<< "    COM/ComPort " << comPort.toStdString() << " \n"
	//	<< "    COM/BaudRate " << baudRate << " \n"
	//	<< "    COM/DataBits " << dataBits << " \n"
	//	<< "    COM/Parity " << parity.toStdString() << " \n"
	//	<< "    COM/StopBits " << stopBits << " \n"
	//	<< "    COM/FlowControl " << flowControl.toStdString() << " \n"
	//	<< "    MyParam/myParam1 " << myParam1 << " \n"
	//	<< "    MyParam/myParam2 " << myParam2 << " \n\n"
	//	<< endl;

}

void Labonatip_tools::saveSettings()
{


	// [default]
	// user = 
	m_settings->setValue("default/user", ui_tools->lineEdit_userName->text());
	// year = 
	m_settings->setValue("default/year", ui_tools->lineEdit_year->text());
	// month = 
	m_settings->setValue("default/month", ui_tools->lineEdit_month->text());

	// [COM]
	// ComName = COM_
	m_settings->setValue("COM/ComName", QString::fromStdString(m_comSettings->name));
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
	// all the items 
	std::vector<QTreeWidgetItem*> commands_vector;
	m_macro->clear();

	// push all the items in the macro table into the command vector
	for (int i = 0; 
		i < ui_tools->treeWidget_macroTable->topLevelItemCount(); 
		++i) {

		// get the current item
		QTreeWidgetItem *item = ui_tools->treeWidget_macroTable->topLevelItem(i);

		if (item->childCount() < 0) { // if no children, just add the line 
			commands_vector.push_back(ui_tools->treeWidget_macroTable->topLevelItem(i));
		}
		else
		{// otherwise we need to traverse the tree
			commands_vector.push_back(ui_tools->treeWidget_macroTable->topLevelItem(i));
			for (int loop = 0; loop < item->text(1).toInt(); loop++) {
				// we need to check how many times we need to run the operations
				// and add the widget to the list
				for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
					commands_vector.push_back(item->child(childrenCount));
				}

			}

		}

	}

	for (int i = 0; i < commands_vector.size(); ++i)
	{

		fluicell::PPC1api::command new_command;

		
		new_command.comando = static_cast<fluicell::PPC1api::command::commands_list>(
			commands_vector.at(i)->text(0).toInt());

		new_command.value = commands_vector.at(i)->text(1).toInt();
		new_command.ask = commands_vector.at(i)->checkState(2);
		new_command.status_message = commands_vector.at(i)->text(2).toStdString();

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::addAllCommandsToMacro ::: size = " 
			<< commands_vector.at(i)->columnCount()
			<< "new_command.comando = " << new_command.comando 
			<< " new_command.value  =  " << new_command.value  << endl;

		m_macro->push_back(new_command);
	}


	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_tools::addAllCommandsToMacro ::: the current macro will run " << m_macro->size() << " commands " << endl;
		


}


bool Labonatip_tools::loadMacro()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	QString fine_name = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);
	
	//TODO: there is no check for validity in macro the loading procedure
	QFile macroFile(fine_name);
	if (macroFile.exists() && macroFile.open(QIODevice::ReadWrite))
	{
		QByteArray content = macroFile.readLine();

		QList<QTreeWidgetItem *> *list = new QList<QTreeWidgetItem *>();
		while (!content.isEmpty())
		{
			QTreeWidgetItem *newItem = new QTreeWidgetItem();
			QTreeWidgetItem *parent = new QTreeWidgetItem();
			QComboBox *comboBox = new QComboBox(this);
			createNewCommand(*newItem, *comboBox);

			if (decodeMacroCommand(content, *newItem)) {
				if (getLevel(*newItem) == 0) // we are at top level
				{
					
					ui_tools->treeWidget_macroTable->addTopLevelItem(newItem);
					comboBox->setCurrentIndex(newItem->text(0).toInt());
					ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					//list->push_back(newItem);
					cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
						<< " top level item " << newItem->text(0).toStdString()
						<< " level " << newItem->text(13).toStdString() << endl;
					parent = newItem->clone();
				}
				if (getLevel(*newItem) > 0)  // we are at the first level
				{
					getLastNode(ui_tools->treeWidget_macroTable, parent);
					cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
						<< " item level " << getLevel(*newItem) << " text " << newItem->text(0).toStdString() << endl;
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
						comboBox->setCurrentIndex(newItem->text(0).toInt());
						ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					}
					if (getLevel(*newItem) == getLevel(*item) + 1) {
						item->addChild(newItem);
						comboBox->setCurrentIndex(newItem->text(0).toInt());
						ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					}
					if (getLevel(*newItem) < getLevel(*item)) {
						QTreeWidgetItem *parentItem = item;
						for (int i = 0; i < getLevel(*item) - getLevel(*newItem); i++) {
							parentItem = parentItem->parent();
						}
						parentItem->parent()->addChild(newItem);
						comboBox->setCurrentIndex(newItem->text(0).toInt());
						ui_tools->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
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

		ui_tools->treeWidget_macroTable->setItemSelected(
			ui_tools->treeWidget_macroTable->topLevelItem(
			ui_tools->treeWidget_macroTable->topLevelItemCount()-1), true);
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

		cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::saveMacro :::: result size " << result.size() << endl;

		for (int i = 0; i < result.size(); i++) {
			for (int j = 0; j < result.at(i).size(); j++)
			{
				//cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
				//<< " element " << i << " is " << result.at(i).at(j).toStdString() << endl;
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


//void Labonatip_tools::visitTree(QTreeWidgetItem *_item) {}

int Labonatip_tools::getLevel(QTreeWidgetItem _item)
{
	int level;

	// for our item structure the level is on the column number 3
	bool success = false;
	level = _item.text(3).toInt(&success);
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
		//		cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		//<< "getLastNode first column " << _item.text(0).toStdString() << endl;
	}
}

void Labonatip_tools::visitTree(QList<QStringList> &_list, QTreeWidget *_tree, QTreeWidgetItem *_item) {

	QStringList _string_list;

	int depth = 0;
	QTreeWidgetItem *parent = _item->parent();
	while (parent != 0) {
		depth++;
		parent = parent->parent();
	}

	int idx = qobject_cast<QComboBox*>(	_tree->itemWidget(_item, 0) )->currentIndex();

	//there is a problem with the size of the column --- fixed for now, change this!
	_string_list.push_back(QString::number(idx));
	_string_list.push_back(_item->text(1));
	_string_list.push_back(QString::number(_item->checkState(2)));
	_string_list.push_back(_item->text(2));
	_string_list.push_back(QString::number(depth)); // push the depth of the command as last

	_list.push_back(_string_list);

	for (int i = 0; i<_item->childCount(); ++i)
		visitTree(_list, _tree, _item->child(i));
}

QList<QStringList> Labonatip_tools::visitTree(QTreeWidget *_tree) {
	QList<QStringList> list;
	for (int i = 0; i < _tree->topLevelItemCount(); ++i){
		
		if (!checkValidity(_tree->topLevelItem(i), 1)) { 
			QMessageBox::information(this, "Warning !", 
				"Check validity failed during macro saving, <br>please check your settings and try again. ");
			list.clear();
			return list;
		}

		visitTree(list, _tree, _tree->topLevelItem(i));
	}
	return list;
}

bool Labonatip_tools::decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item)
{
	QStringList data_string;
	if (_command.at(0) == *"%") {
		// do nothing, just discard the line
		//	cout<< QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() << "  "
		// << "Labonatip_tools::decodeMacroCommand ::: this line belogs to the header" << endl;
	}
	else
	{
		//cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() <<
		//	"Labonatip_tools::decodeMacroCommand ::: this line does not belog to the header " << _command.toStdString() << "  size " << _command.size() << endl;

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

	if (data_string.size() < 4) {
		cerr << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() <<
			"Labonatip_tools::decodeMacroCommand ::: data_string size error - size = " << data_string.size() << endl;
		return false;  // something went wrong
	}


	// fill the qtreewidget item
	_out_item.setText(0, data_string.at(0)); 
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() <<
		"Labonatip_tools::decodeMacroCommand ::: data_string.at(0) " << data_string.at(0).toStdString()  << endl;
	
	_out_item.setText(1, data_string.at(1));
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() <<
		"Labonatip_tools::decodeMacroCommand ::: data_string.at(1) " << data_string.at(1).toStdString() << endl;
	
	
	if (data_string.at(2) == "2") {
		_out_item.setCheckState(2, Qt::CheckState::Checked); 
	}
	else {
		_out_item.setCheckState(2, Qt::CheckState::Unchecked); 
	}
	_out_item.setText(2, data_string.at(3)); 
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() <<
		"Labonatip_tools::decodeMacroCommand ::: data_string.at(3) " << data_string.at(3).toStdString() << endl;
	
	_out_item.setText(3, data_string.at(4));
	cout << QDate::currentDate().toString().toStdString() << "  " << QTime::currentTime().toString().toStdString() <<
		"Labonatip_tools::decodeMacroCommand ::: data_string.at(4) " << data_string.at(4).toStdString() << endl;




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
	header.append(tr("%% Macro Header V. 0.2 \n"));
	header.append(tr("%% file created on dd/mm/yyyy - "));
	header.append(QDate::currentDate().toString());
	header.append(" ");
	header.append(QTime::currentTime().toString());
	header.append(tr("%% \n"));
	header.append(tr("%% Fluicell Lab-on-a-tip macro file description \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% List of parameters: \n"));
	header.append(tr("%%  - Command :  \n"));
	header.append(tr("%%  -       0:  Pressure ON (mbar)           :  Value in mbar\n"));
	header.append(tr("%%  -       1:  Pressure OFF (mbar)          :  Value in mbar\n"));
	header.append(tr("%%  -       2:  Vacuum Switch (mbar)         :  Value in mbar \n"));
	header.append(tr("%%  -       3:  Vacuum Recirculation (mbar)  :  Value in mbar \n"));
	header.append(tr("%%  -       4:  Solution 1 (open/close)      :  \n"));
	header.append(tr("%%  -       5:  Solution 2 (open/close)      :  \n"));
	header.append(tr("%%  -       6:  Solution 3 (open/close)      :  \n"));
	header.append(tr("%%  -       7:  Solution 4 (open/close)      :  \n"));
	header.append(tr("%%  -       8:  Droplet size (%)             :  Set the droplet size in % respect to the default values \n"));
	header.append(tr("%%  -       9:  Flow speed (%)               :  \n"));
	header.append(tr("%%  -       10: Vacuum (%)                   :  \n"));
	header.append(tr("%%  -       11: Loop (num)                   :  All the commands inside the loop will run cyclically \n"));
	header.append(tr("%%  -       12: Sleep (s)                    :  Wait in seconds \n"));
	header.append(tr("%%  -       13: Ask (string)                 :  Ask a message at the end of some operation \n"));
	header.append(tr("%%  -       14: All Off                      :  All the valves 1 - 4 will be closed \n"));
	header.append(tr("%%  -       15: Pumps Off                    :  Set the all the pumps to zero (pressure and vacuum) \n"));
	header.append(tr("%%  -       16: Valve state (HEX)            :  Set the valve state \n"));
	header.append(tr("%%  -       17: Wait sync                    :  Wait a sync signal \n"));
	header.append(tr("%%  -       18: Sync out                     :  Sync with external trigger \n"));
	header.append(tr("%%  - \n"));
	header.append(tr("%%  - value (mbar, %, s) - value to be applied to the command\n"));
	header.append(tr("%%  - status_message (string) \n"));
	header.append(tr("%%  - depth : depth in the tree of the command, all the commands at different layers will run in loop\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%%  - \n"));
	header.append(tr("%%  - Current default values : \n"));
	header.append(tr("%%  -      P ON     = "));
	header.append(QString::number(m_pr_params->p_on_default));
	header.append(tr("\n"));
	header.append(tr("%%  -      P OFF    = "));
	header.append(QString::number(m_pr_params->p_off_default));
	header.append(tr("\n"));
	header.append(tr("%%  -      V Switch = "));
	header.append(QString::number(m_pr_params->v_switch_default));
	header.append(tr("\n"));
	header.append(tr("%%  -      V Recirc = "));
	header.append(QString::number(m_pr_params->v_recirc_default));
	header.append(tr("\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% Command Value status_message depth\n"));
	header.append(tr("%% Follows a line example\n"));
	header.append(tr("%% 13#1#2#message#0#§ \n"));
	header.append(tr("%% Command Value status_message depth\n%"));
	return header;
}


void Labonatip_tools::clearAllCommands() {
	ui_tools->treeWidget_macroTable->clear();
}

Labonatip_tools::~Labonatip_tools() {
  delete ui_tools;
}