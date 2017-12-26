/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolEditor.h"
#include  <QCheckBox>
#include <QtCharts/QValueAxis>
#include <QtCharts/QAbstractAxis>

Labonatip_protocol_editor::Labonatip_protocol_editor(QWidget *parent ):
	QMainWindow (parent),
	ui_p_editor(new Ui::Labonatip_protocol_editor)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::Labonatip_protocol_editor    " << endl;

	ui_p_editor->setupUi(this );
	ui_p_editor->treeWidget_params->resizeColumnToContents(0);
	//ui_p_editor->treeWidget_macroTable->setColumnWidth(0, 350);

	// initialize the macro wizard
	protocolWizard = new Labonatip_macroWizard();

	m_solutionParams = new solutionsParams();
	m_pr_params = new pr_params();

	max_pon = 450;
	max_poff = 450;
	max_v_recirc = 300;
	max_v_switch = 300;


	setGUIcharts();

	// connect GUI elements: macro tab
	connect(ui_p_editor->treeWidget_macroTable,
		SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, 
		SLOT(checkValidity(QTreeWidgetItem *, int)));


	// connect GUI elements: NEW macro tab
	connect(ui_p_editor->pushButton_addMacroCommand,
		SIGNAL(clicked()), this, SLOT(addMacroCommand()));

	connect(ui_p_editor->actionPlus,
		SIGNAL(triggered()), this, SLOT(addMacroCommand()));

	connect(ui_p_editor->pushButton_removeMacroCommand,
		SIGNAL(clicked()), this, SLOT(removeMacroCommand()));

	connect(ui_p_editor->actionMinus,
		SIGNAL(triggered()), this, SLOT(removeMacroCommand()));

	connect(ui_p_editor->pushButton_becomeChild,
		SIGNAL(clicked()), this, SLOT(becomeChild()));

	connect(ui_p_editor->pushButton_becomeParent,
		SIGNAL(clicked()), this, SLOT(becomeParent()));

	connect(ui_p_editor->pushButton_moveDown,
		SIGNAL(clicked()), this, SLOT(moveDown()));

	connect(ui_p_editor->actionDown,
		SIGNAL(triggered()), this, SLOT(moveDown()));

	connect(ui_p_editor->pushButton_moveUp,
		SIGNAL(clicked()), this, SLOT(moveUp()));

	connect(ui_p_editor->actionUp,
		SIGNAL(triggered()), this, SLOT(moveUp()));

	connect(ui_p_editor->pushButton_plusIndent,
		SIGNAL(clicked()), this, SLOT(plusIndent()));

	connect(ui_p_editor->actionIndent,
		SIGNAL(triggered()), this, SLOT(plusIndent()));

	connect(ui_p_editor->pushButton_duplicateLine,
		SIGNAL(clicked()), this, SLOT(duplicateItem()));

	connect(ui_p_editor->actionDuplicate,
		SIGNAL(triggered()), this, SLOT(duplicateItem()));

	connect(ui_p_editor->pushButton_clearCommands,
		SIGNAL(clicked()), this, SLOT(clearAllCommands()));

	connect(ui_p_editor->actionClear,
		SIGNAL(triggered()), this, SLOT(clearAllCommands()));

	connect(ui_p_editor->actionSave,
		SIGNAL(triggered()), this, SLOT(saveMacro()));

	connect(ui_p_editor->actionLoad,
		SIGNAL(triggered()), this, SLOT(loadMacro()));

	connect(ui_p_editor->actionWizard,
		SIGNAL(triggered()), this, SLOT(newProtocolWizard()));

	connect(ui_p_editor->pushButton_openFolder,
		SIGNAL(clicked()), this, SLOT(openProtocolFolder()));

	connect(ui_p_editor->treeWidget_protocol_folder, 
		SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(on_protocol_clicked(QTreeWidgetItem*, int)));

	connect(protocolWizard,
		SIGNAL(loadSettings()), this, SLOT(emitLoadSettings()));

	connect(protocolWizard,
		SIGNAL(loadStdProtocol()), this, SLOT(loadStdP()));

	connect(protocolWizard,
		SIGNAL(loadOptProtocol()), this, SLOT(loadOptP()));

	connect(protocolWizard,
		SIGNAL(loadCustomProtocol()), this, SLOT(loadCustomP()));

	connect(protocolWizard,
		SIGNAL(loadSleepProtocol()), this, SLOT(loadSleepP()));

	connect(protocolWizard,
		SIGNAL(loadAllOffProtocol()), this, SLOT(loadAlloffP()));

	connect(protocolWizard,
		SIGNAL(saveProtocol()), this, SLOT(saveMacro()));

	// connect tool window events Ok, Cancel, Apply
	connect(ui_p_editor->buttonBox->button(QDialogButtonBox::Ok),
		SIGNAL(clicked()), this, SLOT(okPressed()));

	connect(ui_p_editor->buttonBox->button(QDialogButtonBox::Cancel),
		SIGNAL(clicked()), this, SLOT(cancelPressed()));

	connect(ui_p_editor->buttonBox->button(QDialogButtonBox::Apply),
		SIGNAL(clicked()), this, SLOT(applyPressed()));

}


void Labonatip_protocol_editor::okPressed() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::okPressed    " << endl;

	addAllCommandsToMacro();
	//TODO manual save for now
	//saveSettings();
	//TODO other settings ! 

	emit ok();
	this->close();
}

void Labonatip_protocol_editor::cancelPressed() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::cancelPressed    " << endl;

	emit cancel();
	this->close();
}


void Labonatip_protocol_editor::applyPressed() {

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::applyPressed    " << endl;

	addAllCommandsToMacro();
	updateChartProtocol(m_macro);
	//TODO manual save for now
	//saveSettings();
	//TODO other settings ! 

	emit apply();

}

void Labonatip_protocol_editor::newProtocolWizard()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::newProtocolWizard    " << endl;

	protocolWizard->setMacroPath(m_protocol_path);
	protocolWizard->setPrParams(*m_pr_params);
	protocolWizard->setSolParams(*m_solutionParams);

	protocolWizard->setModal(true);
	//protocolWizard->setSolNames();
	//protocolWizard->setDefPreVac();
	protocolWizard->show();

}

void Labonatip_protocol_editor::addMacroCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::addMacroCommand " << endl;
	
	// create a new item
	QTreeWidgetItem *newItem = new QTreeWidgetItem;
	macroCombobox *comboBox = new macroCombobox();
	createNewCommand(*newItem, *comboBox);


	//if we are at the top level with no element or no selection 
	// the element is added at the last position
	if (ui_p_editor->treeWidget_macroTable->topLevelItemCount() < 1 ||
		!ui_p_editor->treeWidget_macroTable->currentIndex().isValid()) {
		ui_p_editor->treeWidget_macroTable->insertTopLevelItem(0, newItem);
		// the combo widget must be created for every item in the tree 
		// and it goes always to the first column
		ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox); // set the combowidget
		ui_p_editor->treeWidget_macroTable->resizeColumnToContents(0);

	}
	else { 	//else we add the item at a specific row
		int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();
		// get the parent
		QTreeWidgetItem *parent = ui_p_editor->treeWidget_macroTable->currentItem()->parent();

		if (ui_p_editor->treeWidget_macroTable->currentItem()->parent()) { // if the parent is valid
			// add the new line as a child
			parent->insertChild(row + 1, newItem);
			ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);// set the combowidget
		}
		else { // add the new line at the row 
			ui_p_editor->treeWidget_macroTable->insertTopLevelItem(row + 1, newItem);
			ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);// set the combowidget
		}

		ui_p_editor->treeWidget_macroTable->resizeColumnToContents(0);

	}

	addAllCommandsToMacro();
	updateChartProtocol(m_macro);
}

void Labonatip_protocol_editor::removeMacroCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::removeMacroCommand    " << endl;

	if (ui_p_editor->treeWidget_macroTable->currentItem() &&
		ui_p_editor->treeWidget_macroTable->topLevelItemCount() > 0) {// avoid crash is no elements in the table or no selection
		// destroy the selected item
		ui_p_editor->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
}

void Labonatip_protocol_editor::becomeChild()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::becomeChild    " << endl;

	if (!ui_p_editor->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection
	
    // create a new item
	QTreeWidgetItem *item = new QTreeWidgetItem();
	macroCombobox *comboBox = new macroCombobox();
	createNewCommand(*item, *comboBox); 
	
	// clone the current selected item
	item = ui_p_editor->treeWidget_macroTable->currentItem()->clone();
	//get the row index
	int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();

	// the item above the current item becomes the parent
	QTreeWidgetItem *parent = 
		ui_p_editor->treeWidget_macroTable->itemAbove(
			ui_p_editor->treeWidget_macroTable->currentItem());
	if(parent){
		parent->addChild(item); // add the clone of the selected item as a child
		ui_p_editor->treeWidget_macroTable->setItemWidget(item, 0, comboBox); // set the combowidget
		parent->setExpanded(true);
		// destroy the selected item
		ui_p_editor->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
}

void Labonatip_protocol_editor::becomeParent()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::becomeParent    " << endl;

	if (!ui_p_editor->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection

	QTreeWidgetItem *item = new QTreeWidgetItem;
	macroCombobox *comboBox = new macroCombobox();
	createNewCommand(*item, *comboBox);

	item = ui_p_editor->treeWidget_macroTable->currentItem()->clone();
	int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();

/*	QTreeWidgetItem *parent = ui_p_editor->treeWidget_macroTable->itemAbove(ui_p_editor->treeWidget_macroTable->currentItem());
	if (parent) {
		parent->addChild(item);
		parent->setExpanded(true);
		ui_p_editor->treeWidget_macroTable->currentItem()->~QTreeWidgetItem();
	}
*/
}


void Labonatip_protocol_editor::moveUp()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::moveUp    " << endl;

	// create a combo
	macroCombobox *comboBox = new macroCombobox();
	createNewCommand(*comboBox);

	// get the current selected item
	QTreeWidgetItem *moveItem = ui_p_editor->treeWidget_macroTable->currentItem();
	int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();

	if (moveItem && row > 0) // if the selection is valid and we are not at the first row
	{
		QTreeWidgetItem *parent = ui_p_editor->treeWidget_macroTable->currentItem()->parent();
		// if we are not at the fist level, so the item has a parent
		if (parent) {
			parent->takeChild(row); // take the child at the row
			parent->insertChild(row - 1, moveItem); // add the selected item one row before
			ui_p_editor->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox); // set the combowidget
		}
		else {
			// if we are on the top level, just take the item 
			ui_p_editor->treeWidget_macroTable->takeTopLevelItem(row);
			// and add the selected item one row before
			ui_p_editor->treeWidget_macroTable->insertTopLevelItem(row - 1, moveItem);
			ui_p_editor->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox); // set the combowidget
		}
		ui_p_editor->treeWidget_macroTable->setCurrentItem(moveItem);
	}
}

void Labonatip_protocol_editor::moveDown()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::moveDown    " << endl;

	// create a combo
	macroCombobox *comboBox = new macroCombobox();
	createNewCommand(*comboBox);

	// get the current selected item
	QTreeWidgetItem *moveItem = ui_p_editor->treeWidget_macroTable->currentItem();

	int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();
	int number_of_items = ui_p_editor->treeWidget_macroTable->topLevelItemCount();

	if (moveItem && row >= 0 && row < number_of_items - 1)
	{
		QTreeWidgetItem *parent = ui_p_editor->treeWidget_macroTable->currentItem()->parent();

		if (parent) {
			int childCount = parent->childCount();
			if (row >= childCount - 1) return; // stops at the last

			parent->takeChild(row);
			parent->insertChild(row + 1, moveItem);
			ui_p_editor->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox);
		}
		else {
			ui_p_editor->treeWidget_macroTable->takeTopLevelItem(row);
			ui_p_editor->treeWidget_macroTable->insertTopLevelItem(row + 1, moveItem);
			ui_p_editor->treeWidget_macroTable->setItemWidget(moveItem, 0, comboBox);
		}
		ui_p_editor->treeWidget_macroTable->setCurrentItem(moveItem);
	}
}


void Labonatip_protocol_editor::plusIndent()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::plusIndent    " << endl;

	// create a new item
	QTreeWidgetItem *newItem = new QTreeWidgetItem;
	macroCombobox *comboBox = new macroCombobox();
	createNewCommand(*newItem, *comboBox);

    // if no item selected, add to the top level
	if (!ui_p_editor->treeWidget_macroTable->currentIndex().isValid()) {
		ui_p_editor->treeWidget_macroTable->insertTopLevelItem(0, newItem);
		ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox); // set the combowidget
		return;
	}
	else { // otherwise it add the item as a child
		QTreeWidgetItem *parent = ui_p_editor->treeWidget_macroTable->currentItem();
		parent->insertChild(0, newItem);
		ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox); // set the combowidget
		return;
	}

}



bool Labonatip_protocol_editor::checkValidity(QTreeWidgetItem *_item, int _column)
{
	// check validity for the element
	int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();

	//if (_column != 1) return false; // perform the check on column 1 only
	// in this way the check will be called any modification 
	// but the check will be performed always on the column number 1
	_column = 1;  
	int idx = 
		qobject_cast<QComboBox*>(
			ui_p_editor->treeWidget_macroTable->itemWidget(_item, 0))->currentIndex();
	
	bool isNumeric;

	switch (idx) {
	case 0: { // check pon
		
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
	case 3: { // check v_r

		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning", 
				"Vacuum recirculation is not a valid number, \n its value must be a positive number in [-300, 0]");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number < m_pr_params->v_recirc_min   ||
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

		int number = _item->text(_column).toInt(&isNumeric);
		if (!isNumeric) {
			QMessageBox::warning(this, "Warning ", " Solution is not a valid number, \n its value must be 0 or 1 ! \n where 0 = open. \nOnly one valve can be open.");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		if (number != 0 && 
			number != 1) {
			QMessageBox::warning(this, "Warning ", 
				" Solution is out of range, \n its value must be 0 or 1 ! \n where 0 = open. \nOnly one valve can be open.");
			_item->setText(_column, QString("0")); // if the value is not valid, reset to zero
			return false;
		}
		break;
	}
	case 8:{
		// Droplet size (%) //TODO: remove this !!!

		qobject_cast<QComboBox*>(ui_p_editor->treeWidget_macroTable->itemWidget(_item, 0))->setCurrentIndex(0);
		QMessageBox::warning(this, "Warning",
			"The droplet size command is not usable now, \n the content will be automatically changed to index 0"); 
		_item->setText(_column, "0"); 

		break;
	}   
	case 9:{
		// Flow speed (%)
		QMessageBox::warning(this, "Warning",
			"The flow speed command is not usable now, \n the content will be automatically changed to index 0");
		_item->setText(_column, "0");
		break;
	}
	case 10: {
		// Vacuum (%) //TODO : remove this 
		QMessageBox::warning(this, "Warning",
			"The vacuum command is not usable now, \n the content will be automatically changed to index 0");
		_item->setText(_column, "0");
		break;
	}
	case 11: {
		// check loops

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
		// no need to check here
		
		break;
	}
	case 14: {
		// all off
		// no need to check here

		break;
	}
	case 15: {
		// pumps off
		// no need to check here

		break;
	}	
	case 16: {
		// Valve state"

		break;
	}	
	case 17: {
		// Wait sync"

		break;
	}	
	case 18: {
		// Sync out"

		break;
	}
	default:{
		// default function active if none of the previous

		break;
	}
	}

	addAllCommandsToMacro();
	updateChartProtocol(m_macro);
	return true;
}

void Labonatip_protocol_editor::commandChanged(int _idx)
{

	if (!ui_p_editor->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection
			
	if (!ui_p_editor->treeWidget_macroTable->focusWidget()) return; // avoid crash if no focus

	if (!qobject_cast<QComboBox*>(
		ui_p_editor->treeWidget_macroTable->focusWidget())) return; // avoid crash

	int idx = qobject_cast<QComboBox*>(
		ui_p_editor->treeWidget_macroTable->focusWidget())->currentIndex();

	int row  = ui_p_editor->treeWidget_macroTable->indexAt(
		ui_p_editor->treeWidget_macroTable->focusWidget()->pos()).row();

	int column = ui_p_editor->treeWidget_macroTable->indexAt(
		ui_p_editor->treeWidget_macroTable->focusWidget()->pos()).column();

	QTreeWidgetItem *currentItem = ui_p_editor->treeWidget_macroTable->itemAt(
		ui_p_editor->treeWidget_macroTable->focusWidget()->pos());

	ui_p_editor->treeWidget_macroTable->itemAt(
		ui_p_editor->treeWidget_macroTable->focusWidget()->pos())->setText(0, QString::number(idx));

	QTreeWidgetItem *currentParent = currentItem->parent();
	int parent = 0;
	if (currentParent) parent = 1;
	//int idx = 1;// ui_p_editor->treeWidget_macroTable->indexOfTopLevelItem(wid);
		//_combo_box.currentIndex();
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::commandChanged :::: idx "
		<< idx 
		<< "  row " << row 
		<< "  column " << column
		<< "  currentParent " << parent
		<< endl;

	checkValidity(ui_p_editor->treeWidget_macroTable->currentItem(), _idx);
}


void Labonatip_protocol_editor::duplicateItem()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::duplicateItem    " << endl;

	if (!ui_p_editor->treeWidget_macroTable->currentItem()) return; // avoid crash if no selection

	// get the current selected item
	QTreeWidgetItem *newItem = 
		ui_p_editor->treeWidget_macroTable->currentItem()->clone();

	int row = ui_p_editor->treeWidget_macroTable->currentIndex().row();
	QTreeWidgetItem *parent = ui_p_editor->treeWidget_macroTable->currentItem()->parent();

	if (parent) {

			parent->insertChild(row + 1, newItem);
			// create a combo
			macroCombobox *comboBox = new macroCombobox();
			createNewCommand(*comboBox);
			// get the index in the combobox of the current item and set it to the new widget
			int idx = qobject_cast<QComboBox*>(
				ui_p_editor->treeWidget_macroTable->itemWidget(
					ui_p_editor->treeWidget_macroTable->currentItem(), 0))->currentIndex();
			comboBox->blockSignals(true);
			comboBox->setCurrentIndex(idx);
			comboBox->blockSignals(false);
			ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
		}
		else {
			
			ui_p_editor->treeWidget_macroTable->insertTopLevelItem(row + 1, newItem);
			// create a combo
			macroCombobox *comboBox = new macroCombobox();
			createNewCommand(*comboBox);
			// get the index in the combobox of the current item and set it to the new widget
			int idx = qobject_cast<macroCombobox*>(
				ui_p_editor->treeWidget_macroTable->itemWidget(
					ui_p_editor->treeWidget_macroTable->currentItem(), 0))->currentIndex();
			comboBox->blockSignals(true);
			comboBox->setCurrentIndex(idx);
			comboBox->blockSignals(false);

			ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
			if (newItem->childCount() > 0)
			{
				for (int i = 0; i < newItem->childCount(); i++)	{

					// create a new combobox for each child
					macroCombobox *comboBox_child = new macroCombobox();
					createNewCommand(*comboBox_child);
					// get the index in the combobox of the current item and set it to the new widget
					int idx = qobject_cast<QComboBox*>(
						ui_p_editor->treeWidget_macroTable->itemWidget(
							ui_p_editor->treeWidget_macroTable->currentItem()->child(i), 0))->currentIndex();
					
					comboBox_child->blockSignals(true);
					comboBox_child->setCurrentIndex(idx);
					comboBox_child->blockSignals(false);
					ui_p_editor->treeWidget_macroTable->setItemWidget(
						newItem->child(i), 0, comboBox_child);
				}
			}
//		}
		return;
		//TODO all the children do not have the combobox
	}
	return;
}

void Labonatip_protocol_editor::createNewCommand(QTreeWidgetItem & _command, macroCombobox & _combo_box)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::createNewCommand    " << endl;

	_command.setText(0, "Command"); // 
	_command.setText(1, "1"); // 
	_command.setCheckState(2, Qt::CheckState::Unchecked); // status message
	_command.setText(2, "go science !!!"); // status message
	_command.setFlags(_command.flags() | (Qt::ItemIsEditable) | (Qt::ItemIsSelectable));

	_combo_box.setFocusPolicy(Qt::StrongFocus);
	_combo_box.addItems(QStringList() << "Pressure ON (mbar)" << "Pressure OFF (mbar)" 
		<< "Vacuum Switch (mbar)" << "Vacuum Recirculation (mbar)"
		<< "Solution 1 (open/close)" << "Solution 2 (open/close)" 
		<< "Solution 3 (open/close)" << "Solution 4 (open/close)"
		<< "Droplet size (%)" << "Flow speed (%)" << "Vacuum (%)" 
		<< "loop" << "Wait (s)" << "Ask"
		<< "All Off" << "Pumps Off" << "Valve state" << "Wait sync" << "Sync out");

	connect(&_combo_box, SIGNAL(currentIndexChanged(int )), 
		this, SLOT(commandChanged(int )));
}



void Labonatip_protocol_editor::addAllCommandsToMacro()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::addAllCommandsToMacro    " << endl;

	// all the items 
	std::vector<QTreeWidgetItem*> commands_vector;
	m_macro->clear();

	// push all the items in the macro table into the command vector
	for (int i = 0; 
		i < ui_p_editor->treeWidget_macroTable->topLevelItemCount(); 
		++i) {

		// get the current item
		QTreeWidgetItem *item = ui_p_editor->treeWidget_macroTable->topLevelItem(i);

		if (item->childCount() < 0) { // if no children, just add the line 
			commands_vector.push_back(ui_p_editor->treeWidget_macroTable->topLevelItem(i));
		}
		else
		{// otherwise we need to traverse the tree
			commands_vector.push_back(ui_p_editor->treeWidget_macroTable->topLevelItem(i));
			for (int loop = 0; loop < item->text(1).toInt(); loop++) {
				// we need to check how many times we need to run the operations
				// and add the widget to the list
				for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
					commands_vector.push_back(item->child(childrenCount));
				}
			}
		}
	}

	for (size_t i = 0; i < commands_vector.size(); ++i)
	{

		fluicell::PPC1api::command new_command;

		new_command.setInstruction( static_cast<fluicell::PPC1api::command::instructions>(
			commands_vector.at(i)->text(0).toInt()));

		new_command.setValue( commands_vector.at(i)->text(1).toInt());
		new_command.setVisualizeStatus( commands_vector.at(i)->checkState(2));
		new_command.setStatusMessage( commands_vector.at(i)->text(2).toStdString());

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_tools::addAllCommandsToMacro ::: size = " 
			<< commands_vector.at(i)->columnCount()
			<< " new_command.instruction = " << new_command.getInstruction()
			<< " new_command.value  =  " << new_command.getValue()  << endl;

		m_macro->push_back(new_command);
	}

	double duration = protocolDuration(*m_macro);
	ui_p_editor->treeWidget_params->topLevelItem(8)->setText(1, QString::number(duration));
	int remaining_time_sec = duration;
	QString s;
	s.append("Protocol duration :  ");
	int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	s.append(" h,   ");
	s.append(QString::number(remaining_mins));
	s.append(" min,   ");
	s.append(QString::number(remaining_secs));
	s.append(" sec   ");
	ui_p_editor->label_protocolDuration->setText(s);

	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() << "  "
		 << "Labonatip_tools::addAllCommandsToMacro ::: the current macro will run " 
		 << m_macro->size() << " commands " 
		 << " its duration is " << duration << endl;

}



bool Labonatip_protocol_editor::loadMacro()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_tools::loadMacro :::  "
		<< m_protocol_path.toStdString() << "  " << endl;
	
	QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), m_protocol_path,  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);
	
	if (file_name.isEmpty()) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning ", 
			"Empty path, file not found ! <br>" + file_name);
		return false;
	}
	
	return loadMacro(file_name);
}


bool Labonatip_protocol_editor::loadMacro(const QString _file_name)
{
	//TODO: there is no check for validity in macro the loading procedure
	QFile macroFile(_file_name);
	m_current_protocol_file_name = _file_name;
	if (macroFile.exists() && macroFile.open(QIODevice::ReadWrite))
	{
		QByteArray content = macroFile.readLine();

		QList<QTreeWidgetItem *> *list = new QList<QTreeWidgetItem *>();
		while (!content.isEmpty())
		{
			QTreeWidgetItem *newItem = new QTreeWidgetItem();
			QTreeWidgetItem *parent = new QTreeWidgetItem();
			macroCombobox *comboBox = new macroCombobox();
			createNewCommand(*newItem, *comboBox);

			if (decodeMacroCommand(content, *newItem)) {
				if (getLevel(*newItem) == 0) // we are at top level
				{

					ui_p_editor->treeWidget_macroTable->addTopLevelItem(newItem);
					comboBox->setCurrentIndex(newItem->text(0).toInt());
					ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					//list->push_back(newItem);
					cout << QDate::currentDate().toString().toStdString() << "  " 
						<< QTime::currentTime().toString().toStdString() << "  "
						<< " top level item " << newItem->text(0).toStdString()
						<< " level " << newItem->text(13).toStdString() << endl;
					parent = newItem->clone();
				}
				if (getLevel(*newItem) > 0)  // we are at the first level
				{
					getLastNode(ui_p_editor->treeWidget_macroTable, parent);
					cout << QDate::currentDate().toString().toStdString() << "  " 
						<< QTime::currentTime().toString().toStdString() << "  "
						<< " item level " << getLevel(*newItem) 
						<< " text " << newItem->text(0).toStdString() << endl;
					QTreeWidgetItem *item = new QTreeWidgetItem();
					QTreeWidgetItemIterator *item_iterator =
						new QTreeWidgetItemIterator(ui_p_editor->treeWidget_macroTable, 
							QTreeWidgetItemIterator::All);
					QTreeWidgetItemIterator it(ui_p_editor->treeWidget_macroTable);
					while (*it) { // this will just get the last node
						item = (*it);
						++it;
					}
					if (getLevel(*item) == getLevel(*newItem)) {
						item->parent()->addChild(newItem);
						comboBox->setCurrentIndex(newItem->text(0).toInt());
						ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					}
					if (getLevel(*newItem) == getLevel(*item) + 1) {
						item->addChild(newItem);
						comboBox->setCurrentIndex(newItem->text(0).toInt());
						ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					}
					if (getLevel(*newItem) < getLevel(*item)) {
						QTreeWidgetItem *parentItem = item;
						for (int i = 0; i < getLevel(*item) - getLevel(*newItem); i++) {
							parentItem = parentItem->parent();
						}
						parentItem->parent()->addChild(newItem);
						comboBox->setCurrentIndex(newItem->text(0).toInt());
						ui_p_editor->treeWidget_macroTable->setItemWidget(newItem, 0, comboBox);
					}
					ui_p_editor->treeWidget_macroTable->update();
				}
				else { // there is something wrong !! 
					   //QMessageBox::warning(this, "Warning !", "Negative level, file corrupted  ! ");
				}
			}
			content = macroFile.readLine();
		}

		//resize columns to content
		for (int i = 0; i < ui_p_editor->treeWidget_macroTable->columnCount(); i++) {
			ui_p_editor->treeWidget_macroTable->resizeColumnToContents(i);
		}

		ui_p_editor->treeWidget_macroTable->setItemSelected(
			ui_p_editor->treeWidget_macroTable->topLevelItem(
				ui_p_editor->treeWidget_macroTable->topLevelItemCount() - 1), true);
		//ui_p_editor->treeWidget_macroTable->addTopLevelItems(*list);

	}
	else {
		QMessageBox::warning(this, "Warning ", 
			"File not found ! <br>" + _file_name);
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		return false;

	}

	addAllCommandsToMacro();
	updateChartProtocol(m_macro);
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return true;
}

bool Labonatip_protocol_editor::saveMacro()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::saveMacro    " << endl;

	QString fileName = QFileDialog::getSaveFileName(this, 
		tr("Save something"), m_protocol_path,  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);

	if (!saveMacro(fileName)) {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning ", "File not saved ! <br>" + fileName);
		return false;
	}
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return true;
}



bool Labonatip_protocol_editor::saveMacro(QString _file_name)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	if (_file_name.isEmpty()) {
		_file_name = QFileDialog::getSaveFileName(this, 
			tr("Save something"), QDir::currentPath(),  // dialog to open files
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

		QList<QStringList> result = visitTree(ui_p_editor->treeWidget_macroTable);

		cout << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() << "  "
			 << "Labonatip_tools::saveMacro :::: result size " << result.size() << endl;

		for (int i = 0; i < result.size(); i++) {
			for (int j = 0; j < result.at(i).size(); j++)
			{
				//cout << QDate::currentDate().toString().toStdString() << "  " 
				//     << QTime::currentTime().toString().toStdString() << "  "
				//     << " element " << i << " is " << result.at(i).at(j).toStdString() << endl;
				stream << result.at(i).at(j) << "#";
			}
			stream << "§" << endl;
		}
	}
	else {
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning ", 
			"File not saved ! <br>" + _file_name);
		return false;
	}

	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
	return true;
}


int Labonatip_protocol_editor::getLevel(QTreeWidgetItem _item)
{
	int level;

	// for our item structure the level is on the column number 3
	bool success = false;
	level = _item.text(3).toInt(&success);
	if (success) return level;
	else return -1;

}


void Labonatip_protocol_editor::getLastNode(QTreeWidget *_tree, QTreeWidgetItem *_item)
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

void Labonatip_protocol_editor::visitTree(QList<QStringList> &_list, QTreeWidget *_tree, QTreeWidgetItem *_item) {

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

QList<QStringList> Labonatip_protocol_editor::visitTree(QTreeWidget *_tree) {
	QList<QStringList> list;
	for (int i = 0; i < _tree->topLevelItemCount(); ++i){
		
		if (!checkValidity(_tree->topLevelItem(i), 1)) { 
			QMessageBox::information(this, "Warning ", 
				"Check validity failed during macro saving, <br>please check your settings and try again. ");
			list.clear();
			return list;
		}

		visitTree(list, _tree, _tree->topLevelItem(i));
	}
	return list;
}

int Labonatip_protocol_editor::interpreteLanguage(QString _language)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::interpreteLanguage    " << endl;

	if (_language == "English")
	{
		return 0;
	}
	if (_language == "Svenska")
	{
		return 1;
	}
	if (_language == "Italiano")
	{
		return 2;
	}
	return 0;
}

bool Labonatip_protocol_editor::decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item)
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
		cerr << QDate::currentDate().toString().toStdString() << "  " 
			 << QTime::currentTime().toString().toStdString() 
			 << "Labonatip_tools::decodeMacroCommand ::: data_string size error - size = " 
			 << data_string.size() << endl;
		return false;  // something went wrong
	}


	// fill the qtreewidget item
	_out_item.setText(0, data_string.at(0)); 
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() 
		 << "Labonatip_tools::decodeMacroCommand ::: data_string.at(0) " 
		 << data_string.at(0).toStdString()  << endl;
	
	_out_item.setText(1, data_string.at(1));
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() 
		 << "Labonatip_tools::decodeMacroCommand ::: data_string.at(1) " 
		 << data_string.at(1).toStdString() << endl;
	
	
	if (data_string.at(2) == "2") {
		_out_item.setCheckState(2, Qt::CheckState::Checked); 
	}
	else {
		_out_item.setCheckState(2, Qt::CheckState::Unchecked); 
	}
	_out_item.setText(2, data_string.at(3)); 
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() 
		 << "Labonatip_tools::decodeMacroCommand ::: data_string.at(3) " 
		<< data_string.at(3).toStdString() << endl;
	
	_out_item.setText(3, data_string.at(4));
	cout << QDate::currentDate().toString().toStdString() << "  " 
		 << QTime::currentTime().toString().toStdString() 
		 << "Labonatip_tools::decodeMacroCommand ::: data_string.at(4) " 
		 << data_string.at(4).toStdString() << endl;

	_out_item.setFlags(_out_item.flags() | (Qt::ItemIsEditable));

	return true;
}

void Labonatip_protocol_editor::setGUIcharts()
{
	m_series_Pon = new QtCharts::QLineSeries();
	*m_series_Pon << QPointF(0.0, 0.0)
		<< QPointF(100.0, 0.0);
	m_series_Pon->setPointsVisible(false);

	m_series_Poff = new QtCharts::QLineSeries();
	*m_series_Poff << QPointF(0.0, 0.0)
		<< QPointF(100.0, 0.0);
	m_series_Poff->setPointsVisible(false);

	m_series_v_s = new QtCharts::QLineSeries();
	*m_series_v_s << QPointF(0.0, 0.0)
		<< QPointF(100.0, 0.0);
	m_series_v_s->setPointsVisible(false);

	m_series_v_r = new QtCharts::QLineSeries();
	*m_series_v_r << QPointF(0.0, 0.0)
		<< QPointF(100.0, 0.0);
	m_series_v_r->setPointsVisible(false);

	m_chart_p_on = new QtCharts::QChart();
	m_chart_p_on->legend()->hide();

	m_chart_p_off = new QtCharts::QChart();
	m_chart_p_off->legend()->hide();

	m_chart_v_s = new QtCharts::QChart();
	m_chart_v_s->legend()->hide();

	m_chart_v_r = new QtCharts::QChart();
	m_chart_v_r->legend()->hide();

	QtCharts::QValueAxis *axisX_pon = new QtCharts::QValueAxis;
	axisX_pon->setRange(0, 100);
	axisX_pon->setTitleText("Simulation time (%)");
	QtCharts::QValueAxis *axisY_pon = new QtCharts::QValueAxis;
	axisY_pon->setRange(0, max_pon);
	axisY_pon->setTitleText(QStringLiteral("<html><head/><body><p>P<span style=\" vertical-align:sub;\">on</span> (mbar)</p></body></html>"));

	QtCharts::QValueAxis *axisX_poff = new QtCharts::QValueAxis;
	axisX_poff->setRange(0, 100);
	axisX_poff->setTitleText("Simulation time (%)");
	QtCharts::QValueAxis *axisY_poff = new QtCharts::QValueAxis;
	axisY_poff->setRange(0, max_poff);
	axisY_poff->setTitleText(QStringLiteral("<html><head/><body><p>P<span style=\" vertical-align:sub;\">off</span> (mbar)</p></body></html>"));

	QtCharts::QValueAxis *axisX_v_s = new QtCharts::QValueAxis;
	axisX_v_s->setRange(0, 100);
	axisX_v_s->setTitleText("Simulation time (%)");
	QtCharts::QValueAxis *axisY_v_s = new QtCharts::QValueAxis;
	axisY_v_s->setRange(0, max_v_recirc);
	axisY_v_s->setTitleText(QStringLiteral("<html><head/><body><p>V<span style=\" vertical-align:sub;\">switch</span> (mbar)</p></body></html>"));


	QtCharts::QValueAxis *axisX_v_r = new QtCharts::QValueAxis;
	axisX_v_r->setRange(0, 100);
	axisX_v_r->setTitleText("Simulation time (%)");
	QtCharts::QValueAxis *axisY_v_r = new QtCharts::QValueAxis;
	axisY_v_r->setRange(0, max_v_switch);
	axisY_v_r->setTitleText(QStringLiteral("<html><head/><body><p>V<span style=\" vertical-align:sub;\">recirc</span> (mbar)</p></body></html>"));


	m_chart_p_on->addSeries(m_series_Pon);
	m_chart_p_off->addSeries(m_series_Poff);
	m_chart_v_s->addSeries(m_series_v_s);
	m_chart_v_r->addSeries(m_series_v_r);

	//m_chart_p_on->createDefaultAxes();
	m_chart_p_on->setAxisX(axisX_pon, m_series_Pon);
	m_chart_p_on->setAxisY(axisY_pon, m_series_Pon);
	m_chart_p_on->setMargins(QMargins(0, 0, 8, 0));
	m_chart_p_on->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chart_p_off->setAxisX(axisX_poff, m_series_Poff);
	m_chart_p_off->setAxisY(axisY_poff, m_series_Poff);
	m_chart_p_off->setMargins(QMargins(0, 0, 8, 0));
	m_chart_p_off->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chart_v_s->setAxisX(axisX_v_s, m_series_v_s);
	m_chart_v_s->setAxisY(axisY_v_s, m_series_v_s);
	m_chart_v_s->setMargins(QMargins(0, 0, 8, 0));
	m_chart_v_s->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chart_v_r->setAxisX(axisX_v_r, m_series_v_r);
	m_chart_v_r->setAxisY(axisY_v_r, m_series_v_r);
	m_chart_v_r->setMargins(QMargins(0, 0, 8, 0));
	m_chart_v_r->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chartView_p_on = new QtCharts::QChartView(m_chart_p_on);
	m_chartView_p_on->setRenderHint(QPainter::Antialiasing);
	m_chartView_p_on->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chartView_p_off = new QtCharts::QChartView(m_chart_p_off);
	m_chartView_p_off->setRenderHint(QPainter::Antialiasing);
	m_chartView_p_off->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chartView_v_s = new QtCharts::QChartView(m_chart_v_s);
	m_chartView_v_s->setRenderHint(QPainter::Antialiasing);
	m_chartView_v_s->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	m_chartView_v_r = new QtCharts::QChartView(m_chart_v_r);
	m_chartView_v_r->setRenderHint(QPainter::Antialiasing);
	m_chartView_v_r->setBackgroundBrush(QBrush(QColor(0xFA, 0xFA, 0xFA)));

	ui_p_editor->gridLayout_14->addWidget(m_chartView_p_on);
	ui_p_editor->gridLayout_15->addWidget(m_chartView_p_off);
	ui_p_editor->gridLayout_12->addWidget(m_chartView_v_r);
	ui_p_editor->gridLayout_13->addWidget(m_chartView_v_s);

	//gridLayout_12  top right
	//gridLayout_13  bottom right

}

void Labonatip_protocol_editor::openProtocolFolder()
{
	QDir path = QFileDialog::getExistingDirectory(this, tr("Open folder"), m_protocol_path);
	setMacroPath(path.path());

}

void Labonatip_protocol_editor::loadStdP()
{

	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append("initialize.macro");
	loadMacro(protocol_path);

}

void Labonatip_protocol_editor::loadOptP()
{
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append("run.macro");
	loadMacro(protocol_path);
}


void Labonatip_protocol_editor::loadSleepP()
{
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append("sleep.macro");
	loadMacro(protocol_path);
}

void Labonatip_protocol_editor::loadAlloffP()
{
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append("alloff.macro");
	loadMacro(protocol_path);
}

void Labonatip_protocol_editor::loadCustomP()
{
	loadMacro();
}

void Labonatip_protocol_editor::on_protocol_clicked(QTreeWidgetItem *item, int column)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_tools::on_protocol_clicked "<< endl;

	QString file = item->text(0);

	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append(file);

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, "Lab-on-a-tip",
			tr("Do you want to clean your workspace before loading the new protocol?\n Click NO to add the macro at the bottom"),
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		// do nothing
	}
	else {
		this->clearAllCommands();
	}

	loadMacro(protocol_path);
}

void Labonatip_protocol_editor::readProtocolFolder(QString _path)
{
	ui_p_editor->treeWidget_protocol_folder->clear();

	ui_p_editor->lineEdit_protocolPath->setText(_path);
	
	QStringList filters;
	filters << "*.macro";

	QDir protocol_path;
	protocol_path.setPath(_path);
	QStringList list = protocol_path.entryList(filters);

	for(int i = 0; i < list.size(); i++ ) // starting from 2 it will not add ./ and ../
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, list.at(i));
		ui_p_editor->treeWidget_protocol_folder->addTopLevelItem(item);
	}

}

double Labonatip_protocol_editor::protocolDuration(std::vector<fluicell::PPC1api::command> _macro)
{
	// compute the duration of the macro
	double macro_duration = 0.0;
	for (size_t i = 0; i < m_macro->size(); i++) {
		if (m_macro->at(i).getInstruction() ==
			fluicell::PPC1api::command::instructions::sleep)
			macro_duration += m_macro->at(i).getValue();
	}

	return macro_duration;
}




QString Labonatip_protocol_editor::createHeader()
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
	header.append(QString::number(190));//TODO (m_pr_params->p_on_default));
	header.append(tr("\n"));
	header.append(tr("%%  -      P OFF    = "));
	header.append(QString::number(21));//TODO(m_pr_params->p_off_default));
	header.append(tr("\n"));
	header.append(tr("%%  -      V Switch = "));
	header.append(QString::number(115));//TODO(m_pr_params->v_switch_default));
	header.append(tr("\n"));
	header.append(tr("%%  -      V Recirc = "));
	header.append(QString::number(115));//TODO(m_pr_params->v_recirc_default));
	header.append(tr("\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% Command Value status_message depth\n"));
	header.append(tr("%% Follows a line example\n"));
	header.append(tr("%% 13#1#2#message#0#§ \n"));
	header.append(tr("%% Command Value status_message depth\n%"));
	return header;
}


void Labonatip_protocol_editor::clearAllCommands() {
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::clearAllCommands    " << endl;

	ui_p_editor->treeWidget_macroTable->clear();
}



void Labonatip_protocol_editor::updateChartProtocol(f_protocol *_macro)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocol_editor::updateChartMacro   " << endl;


	//TODO:: check if _macro is a valid pointer


	m_series_Pon->clear();
	m_series_Poff->clear();
	m_series_v_s->clear();
	m_series_v_r->clear();


	double current_time = 0.0; //!> starts from zero and will be updated according to the duration of the macro
	double max_time_line = 100.0;  //!> the duration is scaled in the interval [0; 100]


   // append zero
	m_series_Pon->append(current_time,
		0.0);  
	m_series_Poff->append(current_time,
		0.0); 
	m_series_v_s->append(current_time,
		0.0); 
	m_series_v_r->append(current_time,
		0.0); // in [50; 60]



	// if the macro is empty it does not update the chart
	//if (m_macro->size() < 1) return;

	double total_duration = protocolDuration(*m_macro);

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::updateChartMacro ::: the complete duration is : " << total_duration << endl;


	for (size_t i = 0; i < _macro->size(); i++) {
		// in every iteration a new segment is added to the chart
		// hence two points are always needed

		switch (_macro->at(i).getInstruction())
		{
		case 0: { // Pon
				  // remove the tail of the chart
			m_series_Pon->remove(m_series_Pon->at(m_series_Pon->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_Pon->at(m_series_Pon->count() - 1).y(); // last added point 
			double second_x = current_time;
			double second_y = _macro->at(i).getValue();  // new point

			m_series_Pon->append(first_x, first_y); // add the fist point
			m_series_Pon->append(second_x, second_y); // add the second point 

													  //the last point is added at each step, and it must be removed every time a new point is added
			m_series_Pon->append(max_time_line, second_y);

			break;
		}
		case 1: { // Poff
				  // remove the tail of the chart
			m_series_Poff->remove(m_series_Poff->at(m_series_Poff->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_Poff->at(m_series_Poff->count() - 1).y(); // last added point 
			double second_x = current_time;
			double second_y = _macro->at(i).getValue();  // new point

			m_series_Poff->append(first_x, first_y); // add the fist point
			m_series_Poff->append(second_x, second_y); // add the second point 

													   //the last point is added at each step, and it must be removed every time a new point is added
			m_series_Poff->append(max_time_line, second_y);

			break;
		}
		case 2: { // v_switch
				  // remove the tail of the chart
			m_series_v_s->remove(m_series_v_s->at(m_series_v_s->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_v_s->at(m_series_v_s->count() - 1).y(); // last added point 
			double second_x = current_time;
			double second_y = - _macro->at(i).getValue();  // new point

			m_series_v_s->append(first_x, first_y); // add the fist point
			m_series_v_s->append(second_x, second_y); // add the second point 

			//the last point is added at each step, and it must be removed every time a new point is added
			m_series_v_s->append(max_time_line, second_y);
			break;
		}
		case 3: { // V_recirc
				  // remove the tail of the chart
			m_series_v_r->remove(m_series_v_r->at(m_series_v_r->count() - 1));

			// the first point is calculated starting from the last value to the new value an the current time
			double first_x = current_time;
			double first_y = m_series_v_r->at(m_series_v_r->count() - 1).y(); // last added point 
			double second_x = current_time;
			double second_y = - _macro->at(i).getValue();  // new point

			m_series_v_r->append(first_x, first_y); // add the fist point
			m_series_v_r->append(second_x, second_y); // add the second point 

														   //the last point is added at each step, and it must be removed every time a new point is added
			m_series_v_r->append(max_time_line, second_y);
			break;
		}
		case 4: { //solution 1
				  

			break;
		}
		case 5: { //solution 2
				  

			break;
		}
		case 6: { //solution 3
			
			break;
		}
		case 7: { //solution 4
			
			break;
		}
		case 8: { //dropletSize 

			break;
		}
		case 9: { //flowSpeed

			break;
		}
		case 10: { //vacuum

			break;
		}
		case 11: { //loop

			break;
		}
		case 12: { //sleep ---- update the current time
			current_time += 100.0 * _macro->at(i).getValue() / total_duration; //the duration is scaled in the interval [0; 100]
			break;
		}
		case 13: { //ask_msg
			
			break;
		}
		case 14: { //allOff

			break;
		}
		case 15: { //pumpsOff

			break;
		}
		case 16: { //setValveState

			break;
		}
		case 17: { //waitSync
			
			break;
		}
		case 18: { //syncOut
			
			break;
		}
		default:
			break;
		}

	}

	m_chart_p_on->update();
	m_chart_p_off->update();
	m_chart_v_s->update();
	m_chart_v_r->update();
}



Labonatip_protocol_editor::~Labonatip_protocol_editor() {
  delete ui_p_editor;
}